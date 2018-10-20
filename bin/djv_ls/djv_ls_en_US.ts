<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="en_US">
<context>
    <name>djv::ls::Application</name>
    <message>
        <location filename="LsApplication.cpp" line="106"/>
        <location filename="LsApplication.cpp" line="145"/>
        <source>Cannot open: &quot;%1&quot;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="LsApplication.cpp" line="200"/>
        <source>%1 %2 %3 %4</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="LsApplication.cpp" line="202"/>
        <source>%1 %2 %3 %4 %5</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="LsApplication.cpp" line="232"/>
        <source>%1 %2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="LsApplication.cpp" line="258"/>
        <source>%1:</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>djv::ls::Context</name>
    <message>
        <location filename="LsContext.cpp" line="123"/>
        <source>-x_info</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="LsContext.cpp" line="124"/>
        <source>-xi</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="LsContext.cpp" line="129"/>
        <source>-file_path</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="LsContext.cpp" line="130"/>
        <source>-fp</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="LsContext.cpp" line="135"/>
        <source>-recurse</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="LsContext.cpp" line="136"/>
        <source>-r</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="LsContext.cpp" line="141"/>
        <source>-hidden</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="LsContext.cpp" line="146"/>
        <source>-columns</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="LsContext.cpp" line="147"/>
        <source>-c</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="LsContext.cpp" line="154"/>
        <source>-sort</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="LsContext.cpp" line="155"/>
        <source>-s</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="LsContext.cpp" line="160"/>
        <source>-reverse_sort</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="LsContext.cpp" line="161"/>
        <source>-rs</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="LsContext.cpp" line="166"/>
        <source>-x_sort_dirs</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="LsContext.cpp" line="167"/>
        <source>-xsd</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="LsContext.cpp" line="189"/>
        <source>djv_ls

    A command line tool for listing directories with file sequences.

    Example output:

    el_cerrito_bart.1k.tiff   File 2.23MB darby rw Mon Jun 12 21:21:55 2006
    richmond_train.2k.tiff    File 8.86MB darby rw Mon Jun 12 21:21:58 2006
    fishpond.1-749.png       Seq 293.17MB darby rw Thu Aug 17 16:47:43 2006

    Key:

    (name)                        (type) (size) (user) (permissions) (time)

    File types:                   Permissions:

    File - Regular file           r - Readable
    Seq  - File sequence          w - Writable
    Dir  - Directory              x - Executable

Usage

    djv_ls [file|directory]... [option]...

    file      - One or more files or image sequences
    directory - One or more directories
    option    - Additional options (see below)

    If no files or directories are given then the current directory will be used.

Options

    -x_info, -xi
        Don&apos;t show information, only file names.
    -file_path, -fp
        Show file path names.
    -recurse, -r
        Descend into sub-directories.
    -hidden
        Show hidden files.
    -columns, -c (value)
        Set the number of columns for formatting output. A value of zero disables formatting.

Sorting Options

    -sort, -s (value)
        Set the sorting. Options = %1. Default = %2.
    -reverse_sort, -rs
        Reverse the sorting order.
    -x_sort_dirs, -xsd
        Don&apos;t sort directories first.
%3
Examples

    List the current directory:

    &gt; djv_ls

    List specific directories:

    &gt; djv_ls ~/movies ~/pictures

    Sort by time with the most recent first:

    &gt; djv_ls -sort time -reverse_sort
</source>
        <translation type="unfinished"></translation>
    </message>
</context>
</TS>
