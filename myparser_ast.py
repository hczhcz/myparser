import os

rep_indent = '    '
rep_sep = os.linesep
rep_colon = ': '
rep_l = '{'
rep_r = '}'


class ASTNode(object):
    def __init__(self, newname):
        self.name = newname


class TextASTNode(ASTNode):
    def __init__(self, newname, newtext):
        super(TextASTNode, self).__init__(newname)
        self.text = newtext

    def get_text(self):
        return self.text

    def get_full(self, indent=0):
        return self.name + rep_colon + self.text  # TODO: improve

    def len(self):
        return len(self.text)


class ListASTNode(ASTNode):
    def __init__(self, newname, newlist):
        super(ListASTNode, self).__init__(newname)
        self.list = newlist

    def get_text(self):
        return ''.join([item.get_text() for item in self.list])

    def get_full(self, indent=0):
        return self.name + rep_colon + rep_l + ''.join([
            rep_sep + rep_indent * (indent + 1) + (
                item.get_full(indent + 1)
            ) for item in self.list
        ]) + rep_sep + rep_indent * indent + rep_r

    def len(self):
        return sum([item.len() for item in self.list])
