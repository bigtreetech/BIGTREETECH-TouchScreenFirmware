### Comments in code
Comments in the code should always be in English. After the comment marks '//' their should be a space.
Example:
```
//Wrong comment
// Correct comment
```

### If else statements
A if else statement should always look like this:
```
if (statementA == checkup)
{
  //Do something for B
}
else if (statementB == checkup)
{
  //Do something for B
}
else
{
  //Default
}
```
_Notice the space in between the if and '('_

When using a if else statement you should always use the curly braces. Even if it's one line!
Some examples:
```
if (whattodo == true) return; // This is correct
if (whattodo == true)
  return;                                // This should be with curly braces
if (whattodo == true)
{
  return;                                //This is correct. 
}
```

Some more examples
| Wrong | Correct |
|-------|---------|
|`if (checka == true)`|`if (checka == true)`|
|`{ if (checkb == false)`|`{`|
|`    doActionA();`|`  if (checkb == false)`|
|`}`|`  {`|
||`    doActionA();`|
||`  }`|
||`}`|
|----|----|
|`if(checha) {doA();return;}`|`if(checka)`|
||`{`|
||`  doA();`|
||`  return;`|
||`}`|
|----|----|
