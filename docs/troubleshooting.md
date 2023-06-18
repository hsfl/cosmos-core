
Troubleshooting
---------------
You may get an error when executing do_cmake like the following
```
bash: ./do_cmake: /bin/bash^M: bad interpreter: No such file or directory
```
this is because the do_cmake file was created on windows and has a different line feeds and carriage returns that unix expects so can't read the file you. We have to fix the line endings so that it can run in linux. One easy way to do this is to open the file on vi and run:

```
:set fileformat=unix ( to change the line endings)
:x (to save and exit)
```

update: this error should be fixed if the .gitattributes is in the repository