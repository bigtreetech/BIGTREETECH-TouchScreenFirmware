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

input_path =  os.path.join("TFT", "src", "User", "API", "Language")
output_path = os.path.join("Copy to SD Card root directory to update", "Language Packs")
# output_path2 = "/Copy to SD Card root directory to update - Unified Menu Material theme/Language Packs"
setting_path = os.path.join("TFT", "src", "User", "API", "Settings.h")

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
    d_path = os.path.join(repo_path, output_path, filename)
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
        val_string = val_string.replace(has_ex.group(), '')
    return val_string

def get_lang_sign():
    global lang_sign
    with open(os.path.join( repo_path, setting_path), 'r', encoding = "utf8") as set_file:
        lines_list = set_file.readlines()
    for text_line in lines_list:
        text_line = text_line.strip()
        if text_line.startswith(lang_sign_prefix):
            l = text_line.split()
            lang_sign = l[2]
    #print("lang sign :" + lang_sign)

try:
    print("Generate language_xx.ini files:")
    repo_path = os.path.realpath("")
    #print(repo_path)
    get_lang_sign()
    # open and scan every .h file found
    for src_file in glob.glob(os.path.join(repo_path, input_path, source_file)):
        key_count = 0
        file_count += 1
        print("Processing: " + get_filename(src_file), end = ": ")
        # create .ini file to start writing keywords
        with open(src_file, 'r', encoding = "utf8") as source_file, open(dest_filepath(src_file), 'w', encoding = "utf8") as dest_file:
            header = string_header.replace("_code_", get_langcode(get_filename(src_file)))
            header = header.replace("_sign_", lang_sign)
            dest_file.write(header)
            # extract keywords from source file
            for line in source_file:
                line = line.strip()
                if line.startswith(line_start):
                    label = make_label(line)
                    val_string = get_string(line)
                    #print(val_string)
                    # write keywords to ini file if within size limits
                    if len(val_string.encode()) <= bytesize:
                        dest_file.write(val_string + '\n')
                        key_count += 1
            # add final new line at the end of the file
            #dest_file.writelines("\n")
            print("Total keys: " + str(key_count))

except Exception as e:
    print("Error Occured: " + str(e))

# %%
