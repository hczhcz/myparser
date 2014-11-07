import os
import re

from myparser_tool import MyParserException
from myparser_tool import map_one, map_one_deep, map_all
from myparser_ast import TextASTNode, ListASTNode

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
syntax_error = '!'

parser_deep = False

root_name = 'root'
space_name = 'space'
keyword_name = 'keyword'
ignore_name = ''
buildin_name = {root_name, space_name, keyword_name}


class SpaceRuleItem(object):
    def __init__(self):
        pass

    def dump(self):
        return syntax_space

    def compile(self, env):
        return lambda val, pos: env[space_name](val, pos)


class TextRuleItem(object):
    def __init__(self, newtext):
        self.text = newtext

    def dump(self):
        return self.text

    def compile(self, env):
        checker = lambda x: x if x == self.text else None

        return lambda val, pos: checker(env[keyword_name](val, pos))


class RefRuleItem(object):
    def __init__(self, newtarget):
        self.target = newtarget

    def dump(self):
        return syntax_ref_l + self.target + syntax_ref_r

    def compile(self, env):
        return lambda val, pos: env[self.target](val, pos)


class ErrorRuleItem(object):
    def __init__(self, newerror):
        self.error = newerror

    def dump(self):
        return syntax_ref_l + self.error + syntax_ref_r

    def compile(self, env):
        return lambda val, pos: MyParserException(self.error).do_raise()


class Rule(object):
    def __init__(self, newname):
        self.name = newname


class ListRule(Rule):
    def __init__(self, newname):
        super(ListRule, self).__init__(newname)
        self.rule = list()

    def add_space(self):
        self.rule[-1].append(SpaceRuleItem())

    def add_text(self, newtext):
        self.rule[-1].append(TextRuleItem(newtext))

    def add_ref(self, newtarget):
        if newtarget.find(syntax_error) >= 0:
            self.rule[-1].append(ErrorRuleItem(newtarget))
        elif newtarget != ignore_name:
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
            raise MyParserException(
                '"' + syntax_ref_r + '" expected but not found'
            )

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

    def compile(self, env):
        self.compiled = [
            [
                item.compile(env) for item in line
            ] for line in self.rule
        ]

        list_match = lambda val, pos, l: map_all(
            l, pos,
            lambda x, xpos: x(val, xpos),
            lambda x, xpos: xpos + x.len(),
            lambda x: ListASTNode(self.name, x)
        )

        if parser_deep:
            return lambda val, pos: map_one_deep(
                self.compiled, lambda x: list_match(val, pos, x)
            )
        else:
            return lambda val, pos: map_one(
                self.compiled, lambda x: list_match(val, pos, x)
            )


class BuildinRule(ListRule):
    def __init__(self, newname):
        super(BuildinRule, self).__init__(newname)
        if not newname in buildin_name:
            raise MyParserException('Bad buildin rule name')

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
            raise MyParserException('Too much regex')
        self.regex = newregex

    def dump(self):
        return syntax_regex + self.name + syntax_regex + syntax_colon\
                            + syntax_return\
                            + syntax_indent + self.regex\
                            + syntax_return

    def compile(self, env):
        self.compiled = re.compile(self.regex)

        checker = lambda x: TextASTNode(
            self.name, x.string[x.start():x.end()]
        ) if x else None

        return lambda val, pos: checker(self.compiled.match(val, pos))
