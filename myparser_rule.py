import os
import re

syntax_indent = '    '
syntax_return = os.linesep + os.linesep
syntax_sep = os.linesep
syntax_colon = ':'
syntax_list = ''
syntax_buildin = '**'
syntax_regex = '*'
syntax_space = ' '
syntax_ref_l = '<'
syntax_ref_r = '>'

re_list = re.compile(r'(?<=^)[\w\d_ ]+(?=:)')
re_buildin = re.compile(r'(?<=^\*\*)[\w\d_ ]+(?=\*\*:)')
re_regex = re.compile(r'(?<=^\*)[\w\d_ ]+(?=\*:)')
re_rule = re.compile(r'(?<=^    )(.|..|[^\/].*|\/[^\/].*|\/\/[^ ].*)')

buildin_name = {'root', 'space', 'keyword'}
ignore_name = ''


class SpaceRuleItem(object):
    def __init__(self):
        pass

    def dump(self):
        return syntax_space

    def compile(self):
        pass


class TextRuleItem(object):
    def __init__(self, newtext):
        self.text = newtext

    def dump(self):
        return self.text

    def compile(self):
        pass


class RefRuleItem(object):
    def __init__(self, newtarget):
        self.target = newtarget

    def dump(self):
        return syntax_ref_l + self.target + syntax_ref_r

    def compile(self):
        pass


class Rule(object):
    def __init__(self, newname):
        self.name = newname


class ListRule(Rule):
    def __init__(self, newname):
        super(ListRule, self).__init__(newname)
        self.rule = []

    def add_space(self):
        self.rule[-1].append(SpaceRuleItem())

    def add_text(self, newtext):
        self.rule[-1].append(TextRuleItem(newtext))

    def add_ref(self, newtarget):
        if newtarget != ignore_name:
            self.rule[-1].append(RefRuleItem(newtarget))

    def add(self, newline):
        self.rule.append(list())

        buf = ''
        mode = 0
        newmode = 0

        for char in newline:
            if mode == 3:
                if char == syntax_ref_r:
                    newmode = 0
                else:
                    buf += char
            elif char == syntax_space:
                newmode = 1
            elif char == syntax_ref_l:
                newmode = 3
            else:
                newmode = 2
                buf += char

            if newmode != mode:
                if mode == 1:
                    self.add_space()
                if mode == 2:
                    self.add_text(buf)
                    buf = ''
                if mode == 3:
                    self.add_ref(buf)
                    buf = ''
                mode = newmode

        if mode == 1:
            self.add_space()
        if mode == 2:
            self.add_text(buf)
        if mode == 3:
            raise '"' + syntax_ref_r + '" expected but not found'

    def dump_list(self):
        return syntax_sep.join([
            syntax_indent + ''.join([
                item.dump() for item in line
            ]) for line in self.rule
        ])

    def dump(self):
        return syntax_list + self.name + syntax_list + syntax_colon\
            + syntax_return\
            + self.dump_list()\
            + syntax_return

    def compile(self):
        pass


class BuildinRule(ListRule):
    def __init__(self, newname):
        super(BuildinRule, self).__init__(newname)
        if not newname in buildin_name:
            raise 'Bad buildin rule name'

    def dump(self):
        return syntax_buildin + self.name + syntax_buildin + syntax_colon\
            + syntax_return\
            + self.dump_list()\
            + syntax_return


class RegexRule(Rule):
    def __init__(self, newname):
        super(RegexRule, self).__init__(newname)

    def add(self, newregex):
        if hasattr(self, 'regex'):
            raise 'Too much regex'
        self.regex = newregex

    def dump(self):
        return syntax_regex + self.name + syntax_regex + syntax_colon\
                            + syntax_return\
                            + syntax_indent + self.regex\
                            + syntax_return

    def compile(self):
        pass
