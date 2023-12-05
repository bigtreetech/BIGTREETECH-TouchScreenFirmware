# Pull request Standards
* We recommend creating separate PRs for unrelated fixes, do not gather them in one single PR. By doing so it will make it easier for our team to focus on eventual issues and to review them.

# Coding Standards

> Coding style and formatting are pretty arbitrary, but it’s much easier to follow if everyone uses the same style. Individuals may not agree with every aspect of the formatting rules, and for some of the rules may take some time to get used to, but it is important that all project contributors follow the style rules so that they can all read and understand everyone’s code easily.

<!-- toc -->

- [Naming:](#Naming)
  * [File Names:](#File-Names)
  * [File formats:](#File-formats)
  * [Functions/subroutines:](#Functionssubroutines)
  * [Use of case:](#Use-of-case)
- [Coding Style:](#Coding-Style)
  * [Spacing](#Spacing)
  * [Indentation](#Indentation)
  * [Brace-style](#Brace-style)
  * [Commenting](#Commenting)
  * [Primitive Types](#Primitive-Types)
  * [Memory Usage](#Memory-Usage)
  * [Preprocessor Directives](#Preprocessor-Directives)

<!-- tocstop -->


## Naming:
**Camel-case:**
- Uses Camel-case naming style. Camel-case is the practice of changing the letter to upper case to denote the start of a new word.
- Examples:
```C
variableFirst
secondVariable
variableThirdName
```

### File Names:
- There should be one include file for each source code file. Each include file should describe a single class or tightly integrated set of classes. The general guideline is for include (.h and .hpp) files to contain definitions and NOT instantiation.
- The include file and source files typically begin with lower case letters. i.e. file classBase.h will contain the definitions and classBase.cpp will contain the source code to its functions.
- When referring to an include file within either another include file or source file, do NOT include the full path. This avoids portability problems created by operating system dependent methods of referencing directory paths. Use compiler command-line flags to include the path. (i.e. -I/path-name)
- Use double quotes ("") for all project include files and angle chevrons (<>) for include files provided by the operating system or compiler.
- To ease use with scripts and make files, file names (and directory paths) should not contain blanks, parentheses "(", or crazy characters not supported by majority of operating systems.

### File formats:
- Use UTF-8 encoding for all files.
- Remove any trailing whitespace at the end of each line.
- Use 2 spaces indent for *.c,*.cpp,*.h and.
- Use Tab indent for *.py,*.conf files.
- Always add a final newline at the end of a file.

**Header files:**
Use the preprocessor to check for multiple inclusions.
```C

#ifndef CLASS_NAME_H
#define CLASS_NAME_H
...
..
#endif
```
If C functions will be called from C++, then C include files will use the following declaration:
```C

#ifdef __cplusplus
extern "C" {
#endif
...
..
C function declarations
..
...
#ifdef __cplusplus
}
#endif
```

### Functions/subroutines:
- Names: Use complement names for complement functions: get/set, add/remove, create/destroy, start/stop, insert/delete, increment/decrement, begin/end, first/last, up/down, min/max, next/previous, open/close, show/hide, suspend/resume, etc.
- Length: It makes no sense to set a rule or hard limit on the length of a function. A human should be able to fully understand the complexity of a single function. If one can't, then it needs to be broken down into a smaller understandable size. It makes no sense to break a 3000 line function into smaller components if all it consists of is a massive case/switch statement which can be grasped upon first viewing. Complex algorithms should be broken up into understandable units and maybe too long at 50 lines of code. Thus the length of a function requires common sense more than a rule which sets a hard limit.
- Add function name as a comment at the end of a lengthy function.
- Example:
```C
void testFunction(int a, int b)
{
  if (a > 0)
  {
    handleVariable(a);
    printf("a > 0");
  }
  if (b > 0)
  {
    handleVariable(b);
    printf("b > 0");
  }
} // testFunction
```

### Use of case:
- Use capital letters with an underscore used to separate words for macro constants and enumerations.
Example: `#define VALUE_OF_PI 3.14`
- Use a single capital letter to denote the start of a new word in the variable, class or function name.
Example: `thisIsAnExample`
- Function names should begin with a lower case letter.



## Coding Style:

Try to keep the lines within **125** characters, because this is the maximum number of characters that you can see in the GitHub diff view.
How long is too long does not only depend on your code editor window, but all of the tools you use to view code. For some old-school programmers, this is only the terminal window, but most programmers now use more tools than just an IDE. Coding is a team sport, and now we have web-based collaboration tools.

> This repository uses some libraries/files from other open source projects which may follow a different coding style. Follow the style of the respective file so that it is easier to track/compare them to the original source.

### Spacing
- One space between keywords and their conditions:
`if (…), while (…), do {…} while (…) etc.`
- No space between functions and their arguments:
`myFunction(…);`
- Operators should have a space on both sides of them:
Example:
this is bad: `variableA>variableB`
this is good: `variableA > variabelB`
- Remove any trailing whitespace at the ends of lines.
- Use spaces for indentation.

**Vertical Spacing**
Sometimes vertical space between line may make complex code more readable, like adding a blank line to differentiate between variable declarations, function calls or blank line before the start of a block.
Adding blank lines also means that fewer code lines will be visible. This makes it difficult to analyse and understand what's going on in the code. Hence, it does not makes sense in adding space when there are only a few lines.
- Do not end functions with blank lines.
- Do not start functions with blank lines.
- Do not use blank lines when you do not have to.

The more code that fits on one screen, the easier it is to follow and understand the control flow of the program. Of course, readability can suffer from code being too dense as well as too spread out, so use your judgment. But in general, minimize the use of vertical whitespace

Example:
```C
// Here it is beneficial to add space between lines
bool updateIcon(void)
{
  uint16_t found = 0;
  uint16_t notfound = 0;
  char nowBmp[64];

  GUI_Clear(infoSettings.bg_color);
  GUI_DispString(5, PADDING, (uint8_t *)"Updating Icons");
  GUI_ClearPrect(&iconUpdateRect);

  for (int i = 0; i < COUNT(iconBmpName); i++)
  {
    sprintf(nowBmp, BMP_ROOT_DIR "/%s.bmp", iconBmpName[i]);
    GUI_ClearPrect(&labelUpdateRect);
    GUI_DispString(labelUpdateRect.x0, labelUpdateRect.y0, (uint8_t *)nowBmp);
    processIcon(nowBmp, ICON_ADDR(i));
  }

  if (notfound == 0)
    return true;
  else
    return false;
}


// Sometimes adding space between line may make the code
// difficult to grasp upon first viewing
void parseData(uint8_t * data)
{
  uint8_t data_type = data[0];

  getIndex(&index);

  if (data[index] == 1)
  {
    data = cache[data_type];

    sendStatus("Step 1");

    finishStep1();
  }
  else if (data[index] == 2)
  {
    data = cache[2];

    sendStatus("Step 2");

    finishStep2();
  }
  else
  {
    data = NULL;

    sendStatus("Step 3");

    finishStep2();
  }
}


// Removing the line spacing make it easier to understand
void parseData(uint8_t * data)
{
  uint8_t data_type = data[0];
  getIndex(&index);

  if (data[index] == 1)
  {
    data = cache[data_type];
    sendStatus("Step 1");
    finishStep1();
  }

  else if (data[index] == 2)
  {
    data = cache[2];
    sendStatus("Step 2");
    finishStep2();
  }
  else
  {
    data = NULL;
    sendStatus("Step 3");
    finishStep3();
  }
}
```

### Indentation
- Use indentation to improve the readability and maintainability of code.
- Use 2 spaces and don’t use tabs.
- All block elements should increase the indentation level, including #if blocks and other non-brace compiler blocks:
```C
void testFunction()
{
  if (variableA == 0)
  {
    #ifdef HAS_SENSOR
      read(a);
    #else
      doThis();
    #endif
  }
}
```
- Break up long Boolean expressions and parameters list onto separate lines with appropriate indentation for alignment
```C
if (veryLongVariableA < veryLongVariableB &&
    veryLongVariableC < veryLongVariableD)
{
...
}
```
- Indent case labels in a switch stamement.

```C
switch (variableA)
  {
    case 0:
    {
      bool variableB = getValue(variableA);
      callFunction(VariableB);
      break;
    }

    case 1:
      callFunction();
      break;

    default:
      break;
  }
```

### Brace-style
Use separate line for Braces. avoid Braces on simple Single-Statement bodies of if/else/loop statements
Try to omit the braces to avoid unnecessary line noise. However, braces should be used in cases where the omission of braces harms the readability and maintainability of the code.
Vertically align closing braces to the opening line.
Use braces on the outer `if` to avoid a potential dangling `else` situation.
Examples (This list is not exhaustive):
```C
// Omit the braces, since the body is simple and clearly associated with the if.
if (variableA < variableB > VariableC)
  handleVariable(B);
else if (variableA < variableD > VariableC)
  handleVariable(D);


  //commenting on a specific condition
  if (variableA < variableB)
  {
    // use braces if a comment in intended for a specific body
    // and the comment need more than one line.
    handleVariable(b);
  }

  //use braces for both `if` and `else` blocks to keep them uniform
  if (a > b)
  {
    handleVariable(b);
  }
  else
  {
    handleVariable(a);
    b = a + d;
    handleVariable(b);
  }

  // Use braces on the outer block because of a nested `if`.
  if (a < getVariable(b) < c)
  {
    if (isValid(b))
      handleVariable(b);
    else
      handleVariable(d);
  }


  // Use braces when a condition/expression is split into multiple lines
  if (veryLongVariableA < veryLongVariableB &&
      veryLongVariableC < veryLongVariableD)
  {
    doSomething();
  }

```

### Commenting
Comments are important for readability and maintainability. When writing comments, write them as English prose, using proper capitalization, punctuation, etc. Aim to describe what the code is trying to do and why not how it does it at a micro-level.
- Use Doxygen-style comments for functions, classes, and other defined entities, and concentrate documentation in the .h files.
```
/**
 * This is an example for multi-line comments to explain
 * anything in detail.
 *
 * Use this style to add comments which will take
 * muntiple lines.
 */
```
- Use C++ single-line style with // for comments under 3 lines.
```
// A short comment that takes up only a line or two
// should just use end-of-line comment style.
```
- If comments are being added on the same line, avoid adding too much spacing between them
Example:
```C
// Too much space before the comments like this will
// make it hard to check for which line the comment is added.

  if (count > 100 && isDone == true)                                          // check if input is complete
  {
    clearBuffer(count);
    processVar();                                                             // Process the input data
    processStr(count);
  }


// making the comments closer to the lines makes it easier to
// understand which line it is explaining.

  if (count > 100 && isDone == true)  // Check if input is complete
  {
    clearBuffer(count);
    processVar();  // Process the input data
    processStr(count);
  }

  //or

  if (count > 100 && isDone == true)  // Check if input is complete
  {
    clearBuffer(count);
    processVar();                     // Process the input data
    processStr(count);
  }


// Adding the comments on a separate line also
// Make it easier to understand

  // Check if input is complete
  if (count > 100 && isDone == true)
  {
    clearBuffer(count);
    // Process the input data
    processVar();
    processStr(count);
  }
```

### Primitive Types
Favour bit-size types like uint8_t and int32_t over short, int, and long. This helps to keep behaviour consistent across architectures.

### Memory Usage
- Avoid using dynamic allocation like `malloc()`.
- Minimise the use of global and static variables.
- Use `const` qualifier to keep data in program memory.
- Do not use a `const` inside a function. If you declared a const inside of a function, every time that function is called, it will allocate a new place in the memory for that const. So you are in a way trying to declare a constant inside a space reserved for a variable. This defeats the whole point of a const.



### Preprocessor Directives
- Use *#define* instead of const for configurable values
- Don’t use #if / #endif for commenting-out unused, old or broken code. We have a git repository! If it’s obsolete, delete it.
- Use #if disabled(FEATURE_NAME) / #endif to compile code for an enabled feature. (Using these macros allows features to be set externally.)
- Use #if enabled(FEATURE_NAME) / #endif to compile code for a disabled feature. See more macros below.
- Use #define macros to avoid repeating boilerplate code.
- Consider both readability and maintainability.
- Label #endif with the opening `#if` condition(s) if the block is over ~15 lines. Make the label compact.
For example, `#endif // TFT35_E3_V3`.
