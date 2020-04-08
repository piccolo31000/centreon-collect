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

package gorgone::standard::constants;

use strict;
use warnings;
use base qw(Exporter);

my %constants;
BEGIN {
    %constants = (
        GORGONE_ACTION_BEGIN => 0,
        GORGONE_ACTION_FINISH_KO => 1,
        GORGONE_ACTION_FINISH_OK => 2,
        GORGONE_STARTED => 3,
    );
}

use constant \%constants;
our @EXPORT;
our @EXPORT_OK = keys %constants;

our %EXPORT_TAGS = ( all => [ @EXPORT_OK ] );

1;
