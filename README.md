Archiver
========

Archiver is an application for PocketBook.

Functionality
----------------------------------------------------

Archiver is an application designed to convert BMP images into an archive file with the extension “.barch”. And as well as converting from “.barch”-files into a BMP images.

Structure of project
---------------------

This project contains two sub-projects:
- The First sub-project contains GUI which based on 'Widgets'
- The second sub-project contains GUI which based on 'QML'


| Archiver   |                 |
| ---------- |:---------------:|
|            |                 |
|            | ArchiverWidgets |
|            |                 |
|            | ArchiverQML     |

Structure of Barch-file
-----------------------

- Signature (BA)
- Image width
- Image height
- Row indexs - array
- Compressed data - array

Development environment
-----------------------
- QtCreator 12.0.2
- Qt 6.5.3 MinGW 64-bit
