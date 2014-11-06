from myparser_rule import ListRule, BuildinRule, RegexRule
from myparser_rule import re_list, re_buildin, re_regex, re_rule


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

    def add_file(self, filename):
        pass

    def dump(self):
        return ''.join([item.dump() for item in self.rule_list])

    def compile(self):
        pass
