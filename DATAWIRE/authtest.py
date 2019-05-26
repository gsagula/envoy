#!/usr/bin/env python

import sys

import requests
import json
import yaml

class HTTPTest (object):
    def __init__(self, target):
        if not "://" in target:
            target = "http://%s" % target

        self.base = target

    def build(self, path=''):
        sep1 = "" if path.startswith("/") else "/"
        sep2 = "" if path.endswith("/") else "/"

        url = "%s%s%s%s" % (self.base, sep1, path, sep2)

        print("URL: %s" % url)

        return url, {}

    def decipher(self, r):
        code = r.status_code
        result = None

        try:
            result = r.text
        except:
            pass

        return code, result

    def get(self, path=''):
        url, args = self.build(path=path)

        return self.decipher(requests.get(url, **args))
    
    def head(self, path=''):
        url, args = self.build(path=path)

        return self.decipher(requests.head(url, **args))

    def put(self, path=''):
        url, args = self.build(path=path)

        return self.decipher(requests.put(url, **args))

    def post(self, path=''):
        url, args = self.build(path=path)

        return self.decipher(requests.post(url, **args))

def ok_based_on_code(code):
    # If it's a 2yz, expect True. If not, expect that we won't get a JSON response 
    # at all.
    return True if ((code // 100) == 2) else None

def run_test(base, test_list):
    q = HTTPTest(base)
    ran = 0
    succeeded = 0
    saved = {}

    for test_info in test_list:
        test_name = test_info['name']
        path = test_info.get('path', '')
        method = test_info.get('method', 'get')
        args = test_info.get('args', {})
        checks = test_info.get('checks')
        updates = test_info.get('updates')

        expected_code = 200

        expect = test_info.get('expect', None)

        if expect:
            if isinstance(expect, int):
                expected_code = expect
            elif isinstance(expect, dict):
                expected_code = expect.get('code', 200)
            else:
                print("%s: bad 'expected' value '%s'" % (test_name, json.dumps(expect)))
                continue

        auth_name = test_info.get('auth', None)

        if auth_name == 'default':
            args['username'] = 'username'
            args['password'] = 'password'
        elif auth_name:
            print("%s: bad 'auth' value '%s'" % (test_name, auth_name))
            continue

        fn = getattr(q, method)

        interpolated_args = {
            'path': path
        }

        missed_values = 0

        for name, value in args.items():
            if isinstance(value, str) and value.startswith("$"):
                aname = value[1:]
                value = saved.get(aname, None)

                if not value:
                    print("%s: saved variable %s for %s is empty" % (test_name, aname, name))
                    missed_values += 1

            interpolated_args[name] = value

        code, result = fn(**interpolated_args)

        # print("%s got %d, %s" % (test_name, code, result))

        ran += 1

        if code != expected_code:
            print("%s: wanted %d, got %d" % (test_name, expected_code, code))
            continue

        print("%s => %d: passed" % (test_name, expected_code))
        succeeded += 1

    print("Ran       %d" % ran)
    print("Succeeded %d" % succeeded)
    print("Failed    %d" % (ran - succeeded))

    return ran - succeeded

if __name__ == "__main__":
    base = sys.argv[1]
    yaml_path = sys.argv[2]
    iterations = 1

    test_list = yaml.safe_load(open(yaml_path, "r"))

    if 'tests' in test_list:
        if 'iterations' in test_list:
            iterations = test_list['iterations']

        test_list = test_list['tests']
    
    for i in range(iterations):
        failed = run_test(base, test_list)

        if failed > 0:
            sys.exit(1)
            
    sys.exit(0)
