This is/will be the CLI version of KKEdit with ( eventually! ) most of the good bits from KKEdit, still lots to do ... watch this space.


This is a TEXT editor and although it can open some files that include binary data, it may not show the correct text, if you want a binary editor I would suggest you use somthing from here:
http://en.wikipedia.org/wiki/Comparison_of_hex_editors
Or you can use vim, see the man page on your system.

See the INSTALL file for details about installing and dependencies.

Bugs and suggestions welcome please email to keithdhedger@gmail.com
For general info on compiling/installing software on your disro see the relevant forums.

Quick start.


Multiple file can be opened, a new tab will be made and switched to automatically.

Menus:

Press 'ESC' to open menus, left/right to switch menu, UP/DOWN/PAGEUP/PAGEDOWN/HOME/END to navigate to sub menu, 'RETURN' to select menu item or 'ESC' to close menu with no selection.
Long menus will scroll up or down as you navigate.

Temporarily removed -->

Main menus can be selected via 'CNTRL+key' where 'key' is the shortcut key ( underscored or highlighted ), eg to open 'FILE' menu 'CNTRL+f'.

<-- Temporarily removed


Menu items in static menus can bew select by just pressing the shortcut key while the menu is open, ie to save a file press: 'CNTRL+f' then 's'.


Edit Menu:

Copy/Cut word under cursor.

Copy/Cut line under cursor.

Paste/


Tabs Menu:
Select tab.

Navigation Menu:

Goto Define - Place cursor over word and select to goto the definition of the functio, ( for now only in the current tab ).

Open Include - Place cursor anywhere on the line with a 'c' type include directive and select to search and open matching files, ( "" open's local include, <> open's global includes from /usr/include ).

Goto Line - Guess!

Open Manpage - place cursor over word and attempt to open the relevent manpage if available in a new tab.

Find - Guess!

Find Next - Find next and wrap to beigining of file if needed.

Function Menu:
... - Dynamic menu containg functions, variable defs etc in the current file, select to goto the define.

Bookmarks:
Remove All Marks - Remove all bookmarks.
Toggle BM - Toggles a bookmark at the current line.
... - Select a bookmark, to go to that tab and line, closing a tab also deletes the bookmarks for that tab.

Tools:
TODO.

Most static menus now have a Ctrl+key shortcut.








