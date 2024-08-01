import json
import re
from subprocess import getoutput

from robot.api import logger


def ctn_files_contain_same_json(file_e: str, file_b: str):
    """
    ctn_files_contain_same_json

    Given two files with md5 as rows, this function checks the first list
    contains the same md5 as the second list. There are exceptions in these files
    that's why we need a function to make this test.

    Args:
        file_e (str): The log file produced by Engine
        file_b (str): The log file produced by Broker

    Returns:
        A boolean that is True on success, False otherwise.
    """
    new_inst = '{"_type": 4294901762, "category": 65535, "element": 2, "broker_id": 1, "broker_name": "",' \
               '"enabled": True, "poller_id": 1, "poller_name": "Central"}'.upper()

    f1 = open(file_e)
    content1 = f1.readlines()

    f2 = open(file_b)
    content2 = f2.readlines()

    idx1 = 0
    idx2 = 0

    r = re.compile(r"^[^{]* (\{.*\})$")
    while idx1 < len(content1) and idx2 < len(content2):
        m1 = r.match(content1[idx1])
        if m1 is not None:
            c1 = m1.group(1)
            if c1.upper() == new_inst:
                continue
        else:
            logger.console("content at line {} of '{}' is not JSON: {}".format(
                idx1, file_e, content1[idx1]))
            idx1 += 1
            continue
        m2 = r.match(content2[idx2])
        if m2 is not None:
            c2 = m2.group(1)
            if c2.upper() == new_inst:
                continue
        else:
            logger.console("content at line {} of '{}' is not JSON: {}".format(
                idx2, file_b, content2[idx2]))
            idx2 += 1
            continue

        if c1 == c2:
            idx1 += 1
            idx2 += 1
        else:
            js1 = json.loads(c1)
            js2 = json.loads(c2)
            if js2['_type'] in [4294901762, 131081]:
                idx2 += 1
                continue
            if js1['_type'] in [4294901762, 131081]:
                idx1 += 1
                continue

            if len(js1) != len(js2):
                logger.console(f"Line {idx1} in '{file_e}' and line {idx2} in '{file_b}' do not match, json contents are respectively\n{js1}\n{js2}")
                return False
            for k in js1:
                if isinstance(js1[k], float):
                    if abs(js1[k] - js2[k]) > 0.1:
                        logger.console(
                            f"Line {idx1} in '{file_e}' and line {idx2} in '{file_b}' do not match, json contents are respectively\n{js1}\n{js2}")
                        return False
                else:
                    if js1[k] != js2[k]:
                        logger.console(
                            f"Line {idx1} in '{file_e}' and line {idx2} in '{file_b}' do not match, json contents are respectively\n{js1}\n{js2}")
                        return False
            idx1 += 1
            idx2 += 1
    retval = idx1 == len(content1) or idx2 == len(content2)
    if not retval:
        logger.console(f"not at the end of files idx1 = {idx1}/{len(content1)} or idx2 = {idx2}/{len(content2)}")
        return False
    return True


def ctn_files_contain_same_md5_1(file_e: str, file_b: str):

    getoutput("awk '{{print $8}}' {0} > {0}.md5".format(file_e))
    getoutput("awk '{{print $8}}' {0} > {0}.md5".format(file_b))

    f1 = open("{}.md5".format(file_e))
    content1 = f1.readlines()

    f2 = open("{}.md5".format(file_b))
    content2 = f2.readlines()

    idx1 = 0
    idx2 = 0

    while idx1 < len(content1) and idx2 < len(content2):
        if content1[idx1] == "test1.lua\n":
            idx1 += 1

        if content2[idx2] == "test.lua\n":
            idx2 += 1
            if content2[idx2] == "055b1a6348a16305474b60de439a0efd\n":
                idx2 += 1
            else:
                return False

        if content1[idx1] == content2[idx2]:
            idx1 += 1
            idx2 += 1
        else:
            print("We have to improve comparaison here")
            return False
    return idx1 == len(content1) and idx2 == len(content2)

##
# @brief Given two files generated by a stream connector, this function checks
# that no events except special ones are sent / replayed twice.
#
# @param str The first file name
# @param str The second file name
#
# @return A boolean True on success


def ctn_check_multiplicity_when_broker_restarted(eng_file: str, broker_file: str):
    f1 = open(eng_file)
    content1 = f1.readlines()
    f2 = open(broker_file)
    content2 = f2.readlines()

    r = re.compile(r".* INFO: ([0-9]+)\s+([0-9a-f]+)\s(.*)$")

    def create_md5_list(content):
        lst = dict()
        typ = dict()
        for l in content:
            m = r.match(l)
            if m:
                type, md5, js = int(m.group(1)), m.group(2), m.group(3)
                if type not in [65544,          # host check
                                4294901762,     # bbdo category
                                196613,         # index mapping => produced on new conf
                                196619,         # pb index mapping => produced on new conf
                                65555,          # service check
                                0x1001a,        # responsive_instance => periodically generated by cbd
                                0x10027,        # pb_host_check
                                0x10028,        # pb_service_check
                                0x1002e,        # pb_responsive_instance
                                0x10036         # pb_instance_configuration
                                ]:
                    if md5 in lst:
                        lst[md5] += 1
                    else:
                        lst[md5] = 1
                        typ[md5] = type
        return lst, typ

    lst1, typ1 = create_md5_list(content1)
    lst2, typ2 = create_md5_list(content2)

    res1 = set(lst1.values())
    res2 = set(lst2.values())
    if len(res1) != 1 or len(res2) != 1:
        for k in lst1:
            if lst1[k] != 1:
                logger.console(
                    "In lst1: Bad {} {} with type {:x}".format(k, lst1[k], typ1[k]))
        for k in lst2:
            if lst2[k] != 1:
                logger.console(
                    "In lst2: Bad {} {} with type {:x}".format(k, lst2[k], typ2[k]))
    return len(res1) == 1 and len(res2) == 1

def ctn_check_multiplicity_when_engine_restarted(file1: str, file2: str):
    """
    ctn_check_multiplicity_when_engine_restarted

    Given two files generated by a stream connector, this function checks
    that no events except special ones are sent / replayed twice.

    Args:
        str The first file name
        str The second file name

    Returns:
        A boolean True on success
    """

    f1 = open(file1)
    content1 = f1.readlines()
    f2 = open(file2)
    content2 = f2.readlines()

    r = re.compile(r".* INFO: ([0-9]+)\s+([0-9a-f]+)\s(.*)$")

    def create_md5_list(content):
        lst = dict()
        typ = dict()
        for l in content:
            m = r.match(l)
            if m:
                type, md5, js = int(m.group(1)), m.group(2), m.group(3)
                """
                 Are removed:
                   * instance configurations
                   * modules
                   * host checks (they can be done several times
                   * host and service events are also configuration events, so sent each time engine is restarted.
                   * local::pb_stop (this one is special and cannot pass through
                     a network).
                """
                if type not in [65544,          # host check
                                4294901762,     # bbdo category
                                65554,          # module
                                65561,          # instance configuration
                                65555,          # service check
                                0x10017,        # service
                                0x1000c,        # host
                                0x1001b,        # pb_service
                                0x1001e,        # pb_host
                                0x1002b,        # pb_module
                                0x10025,        # pb_custom_variable
                                0x10027,        # pb_host_check
                                0x10028,        # pb_service_check
                                0x10036,        # pb_instance_configuration
                                0x90001,        # local::pb_stop
                                ]:
                    if md5 in lst:
                        lst[md5] += 1
                    else:
                        lst[md5] = 1
                        typ[md5] = type
        return lst, typ

    lst1, typ1 = create_md5_list(content1)
    lst2, typ2 = create_md5_list(content2)

    res1 = set(lst1.values())
    res2 = set(lst2.values())
    if len(res1) != 1 or len(res2) != 1:
        for k in lst1:
            if lst1[k] != 1:
                logger.console(
                    "In lst1: Bad {} {} with type {:x}".format(k, lst1[k], typ1[k]))
        for k in lst2:
            if lst2[k] != 1:
                logger.console(
                    "In lst2: Bad {} {} with type {:x}".format(k, lst2[k], typ2[k]))
    return len(res1) == 1 and len(res2) == 1

