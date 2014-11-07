import re

from myparser_tool import MyParserException
from myparser_rule import ListRule, BuildinRule, RegexRule
from myparser_rule import root_name

re_list = re.compile(r'(?<=^)[\w\d_ ]+(?=:)')
re_buildin = re.compile(r'(?<=^\*\*)[\w\d_ ]+(?=\*\*:)')
re_regex = re.compile(r'(?<=^\*)[\w\d_ ]+(?=\*:)')
re_rule = re.compile(r'(?<=^    )(?!\/\/ ).*')


class MyParser(object):
    def __init__(self):
        self.rule_list = list()

    def add_rules(self, rule):
        for item in rule:
            result = re_list.findall(item)
            if len(result) == 1:
                self.rule_list.append(ListRule(result[0]))

            result = re_buildin.findall(item)
            if len(result) == 1:
                self.rule_list.append(BuildinRule(result[0]))

            result = re_regex.findall(item)
            if len(result) == 1:
                self.rule_list.append(RegexRule(result[0]))

            result = re_rule.findall(item)
            if len(result) == 1:
                self.rule_list[-1].add(result[0])

    def dump(self):
        return ''.join([item.dump() for item in self.rule_list])

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
