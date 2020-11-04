#%%
# Auto generate language_xx.ini files from language keyword header files.
# ** Author: Gurmeet Athwal https://github.com/guruathwal **

import os
import re
import glob
import shutil

bytesize = 250                                      # Max string size in bytes
old_prefix = "string_"                              # Old Prefix of defined keyword
new_prefix = "label_"                               # New Prefix for keyword name
source_file = "language_??.h"                       # Source file name wilecard
line_start = "#define STRING_"                      # Starting characters for key line to be processed
lang_sign_prefix = "#define LANGUAGE_FLASH_SIGN"    # Prefix to identify language Sign string
reg_ex = r"([\\][u][0-F]{4}\s?)"                    # regex to find escape sequence for unicode characters

#Header string to write in ini file
string_header = """#### Language Code:_code_
## Language Version:_sign_
## Maximum byte per keyword is 250 Bytes.
## Escape characters are not supported except newline '\\n'\n\n"""

input_path = "/TFT/src/User/API/Language"
output_path = "/Copy to SD Card root directory to update/Language Packs"
output_path2 = "/Copy to SD Card root directory to update - Unified Menu Material theme/Language Packs"
setting_path = "/TFT/src/User/API/Settings.h"

file_count = 0
key_count = 0
lang_sign = ""
repo_path = ""

def get_langcode(line):
    code = line[-4:-2]
    return code.upper()

def get_filename(line):
    line = os.path.split(line)
    return ''.join(line[1])

def dest_filepath(line):
    filename = get_filename(line).replace(".h", ".ini")
    d_path = repo_path + output_path + "/" + filename
    return d_path

def make_label(line):
    word_list = line.split()
    line = word_list[1].lower()
    return line.replace(old_prefix, new_prefix)

def get_defined_name(line):
    word_list = line.split()
    return word_list[1]

def get_string(line):
    str_array = line.rsplit('"')
    val_string = label + ':' + str_array[1]
    has_ex = re.search(reg_ex, val_string)
    if has_ex:
        val_string = val_string.replace(has_ex.group(),'')
    return val_string

def get_lang_sign():
    global lang_sign
    set_file = open(repo_path + setting_path, 'r', encoding="utf8")
    lines_list = set_file.readlines()
    for text_line in lines_list:
        text_line = text_line.strip()
        if text_line.startswith(lang_sign_prefix):
                l = text_line.split()
                lang_sign = l[2]
    #print("lang sign :" + lang_sign)
    set_file.close()

    #####
try:
    print("Generate language_xx.ini files:")
    repo_path = os.path.realpath("")
    #print(repo_path)
    get_lang_sign()

    for src_file in glob.glob(repo_path + input_path + "/" + source_file):
        key_count = 0
        file_count += 1
        print("Processing: " + get_filename(src_file), end =": ")
        source_file = open(src_file, 'r', encoding="utf8")
        dest_file = open(dest_filepath(src_file), 'w', encoding="utf8")
        header = string_header.replace("_code_",get_langcode(get_filename(src_file)))
        header = header.replace("_sign_",lang_sign)
        dest_file.writelines(header)
        lines_list = source_file.readlines()

        for text_line in lines_list:
            text_line = text_line.strip()
            if text_line.startswith(line_start):
                key_count+=1
                label = make_label(text_line)
                val_string = get_string(text_line)
                #print(val_string)
                if len(val_string.encode('utf-8')) > bytesize:
                    raise Exception(">> Size of key '" + get_defined_name(text_line) + "' in " + get_filename(src_file) + " is larger than " + str(bytesize) + " Bytes!\n")
                dest_file.writelines(val_string + "\n")

        dest_file.writelines("\n") #add new line at the end of the file
        source_file.close()
        dest_file.close()
        shutil.copy(dest_filepath(src_file), repo_path + output_path2) #copy file to second folder
        print("Total keywords found:" + str(key_count) + ", File generated:" + get_filename(dest_filepath(src_file)))

    if file_count == 0:
        print("No files found.")
    else:
        print("Total language files processed: " + str(file_count))

except:
    print("Unable to get correct path")
# %%
