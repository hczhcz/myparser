MyParser
===

A parser generator.

`Markdown ===(Python)==> C++11.`

Input Syntax
---

**root**:

    <> <blocks> <>

**space**:

    <*comment line>

**keyword**:

    <id>
    <sign>

blocks:

    <+block>

block:

    <list head> <list body> <>
    <builtin head> <list body> <>
    <regex head> <regex line> <>

list head:

    <id>:<line break>

builtin head:

    **<id>**:<line break>

regex head:

    *<id>*:<line break>

list body:

    <list line> <list body>
    <list line>

list line:

    <indent><list items><line break>

regex line:

    <indent><anything><line break>

comment line:

    <indent>//<anything><line break>
    <other markdown><line break>
    <line break>

list items:

    <*list item>

list item:

    \ <>
    <empty item>
    <keyword item>
    <ref item>
    <error item>

empty item:

    \<<>\>

*keyword item*:

    ([^ \r\n\\<]|\\.)+

ref item:

    \<<id>\>
    \<?<id>\>
    \<*<id>\>
    \<+<id>\>

error item:

    \<!<id>\>

*id*:

    \w([\w ]*\w)?

*sign*:

    \**:|\*+| +|\/\/|<[\*\+\?\!]?|>

*other markdown*:

    ( ? ? ?)[^ \r\n].*[^:\r\n]|.|

*indent*:

    (    )

*line break*:

    [\r\n]+

*anything*:

    .*

Example
---

[gaocegege/CompilerLab (MyLang Project)](https://github.com/gaocegege/CompilerLab)
