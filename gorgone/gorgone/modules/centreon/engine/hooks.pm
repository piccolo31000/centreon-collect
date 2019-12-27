# 
# Copyright 2019 Centreon (http://www.centreon.com/)
#
# Centreon is a full-fledged industry-strength solution that meets
# the needs in IT infrastructure and application monitoring for
# service performance.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

package gorgone::modules::centreon::engine::hooks;

use warnings;
use strict;
use JSON::XS;
use gorgone::class::core;
use gorgone::modules::centreon::engine::class;

use constant NAMESPACE => 'centreon';
use constant NAME => 'engine';
use constant EVENTS => [
    { event => 'ENGINEREADY' },
    { event => 'ENGINECOMMAND', uri => '/command', method => 'POST' },
];

my $config_core;
my $config;
my $engine = {};
my $stop = 0;

sub register {
    my (%options) = @_;
    
    $config = $options{config};
    $config_core = $options{config_core};
    $config->{command_file} = defined($config->{command_file}) ? $config->{command_file} : '/var/lib/centreon-engine/rw/centengine.cmd';
    return (1, NAMESPACE, NAME, EVENTS);
}

sub init {
    my (%options) = @_;

    create_child(logger => $options{logger});
}

sub routing {
    my (%options) = @_;

    my $data;
    eval {
        $data = JSON::XS->new->utf8->decode($options{data});
    };
    if ($@) {
        $options{logger}->writeLogError("[engine] -hooks- Cannot decode json data: $@");
        gorgone::standard::library::add_history(
            dbh => $options{dbh},
            code => 30, token => $options{token},
            data => { msg => 'gorgoneengine: cannot decode json' },
            json_encode => 1
        );
        return undef;
    }
    
    if ($options{action} eq 'ENGINEREADY') {
        $engine->{ready} = 1;
        return undef;
    }
    
    if (gorgone::class::core::waiting_ready(ready => \$engine->{ready}) == 0) {
        gorgone::standard::library::add_history(
            dbh => $options{dbh},
            code => 30, token => $options{token},
            data => { msg => 'gorgoneengine: still no ready' },
            json_encode => 1
        );
        return undef;
    }
    
    gorgone::standard::library::zmq_send_message(
        socket => $options{socket},
        identity => 'gorgoneengine',
        action => $options{action},
        data => $options{data},
        token => $options{token},
    );
}

sub gently {
    my (%options) = @_;

    $stop = 1;
    $options{logger}->writeLogInfo("[engine] -hooks- Send TERM signal");
    if ($engine->{running} == 1) {
        CORE::kill('TERM', $engine->{pid});
    }
}

sub kill {
    my (%options) = @_;

    if ($engine->{running} == 1) {
        $options{logger}->writeLogInfo("[engine] -hooks- Send KILL signal for pool");
        CORE::kill('KILL', $engine->{pid});
    }
}

sub kill_internal {
    my (%options) = @_;

}

sub check {
    my (%options) = @_;

    my $count = 0;
    foreach my $pid (keys %{$options{dead_childs}}) {
        # Not me
        next if ($engine->{pid} != $pid);
        
        $engine = {};
        delete $options{dead_childs}->{$pid};
        if ($stop == 0) {
            create_child(logger => $options{logger});
        }
    }
    
    $count++  if (defined($engine->{running}) && $engine->{running} == 1);
    
    return $count;
}

sub broadcast {
    my (%options) = @_;

    routing(%options);
}

# Specific functions
sub create_child {
    my (%options) = @_;
    
    $options{logger}->writeLogInfo("[engine] -hooks- Create module 'engine' process");
    my $child_pid = fork();
    if ($child_pid == 0) {
        $0 = 'gorgone-engine';
        my $module = gorgone::modules::centreon::engine::class->new(
            logger => $options{logger},
            config_core => $config_core,
            config => $config,
        );
        $module->run();
        exit(0);
    }
    $options{logger}->writeLogInfo("[engine] -hooks- PID $child_pid (gorgone-engine)");
    $engine = { pid => $child_pid, ready => 0, running => 1 };
}

1;
