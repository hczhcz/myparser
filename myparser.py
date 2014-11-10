import os
import re

from myparser_tool import MyParserException
from myparser_rule import RuleList, RuleBuiltin, RuleRegex
from myparser_rule import root_name

re_list = re.compile(r'(?<=^)[\w ]+(?=:)')
re_builtin = re.compile(r'(?<=^\*\*)[\w ]+(?=\*\*:)')
re_regex = re.compile(r'(?<=^\*)[\w ]+(?=\*:)')
re_rule = re.compile(r'(?<=^    )(?!\/\/ ).*')


class MyParser(object):
    def __init__(self):
        self.rule_list = list()

    def add_rules(self, rule):
        for item in rule:
            result = re_list.search(item)
            if result:
                self.rule_list.append(RuleList(
                    result.string[result.start():result.end()]
                ))

            result = re_builtin.search(item)
            if result:
                self.rule_list.append(RuleBuiltin(
                    result.string[result.start():result.end()]
                ))

            result = re_regex.search(item)
            if result:
                self.rule_list.append(RuleRegex(
                    result.string[result.start():result.end()]
                ))

            result = re_rule.search(item)
            if result:
                if len(self.rule_list) == 0:
                    raise MyParserException('Rule name undefined')
                else:
                    self.rule_list[-1].add(
                        result.string[result.start():result.end()]
                    )

    def dump(self):
        return os.linesep.join([
            item.dump() for item in self.rule_list
        ])

    def xdump(self, template):
        return os.linesep.join([
            item.xdump(template) for item in self.rule_list
        ])

    def compile(self):
        self.compiled = dict()
        for item in self.rule_list:
            self.compiled[item.name] = item.compile(self.compiled)

        return self.compiled[root_name]

    def match(self, data):
        result = self.compiled[root_name](data, 0)

        if result:
            return result
        else:
            raise MyParserException('Match nothing')
