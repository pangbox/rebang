# cvdump

`cvdump.exe` is Microsoft's tool for dumping information out of CodeView/PDB files. It is provided here verbatim, for convenience. It is distributed under the MIT license, see `LICENSE`.

It would be convenient if we could use `llvm-pdbutil` instead, but we can't: it doesn't support these very old VC7.1 PDBs.

Some useful options:

- `-l`: C11 line tables
- `-s`: TPI type stream
- `-m`: modules
- `-g`: globals
- `-p`: publics
- `-seccontrib`: section contributions
- `-M<n>`: Select module `<n>`, where `<n>` is a 1-based, decimal module index. Since it's 1-based, it's one larger than the module numbers in `docs/module-list.csv`, e.g. `538` in `docs/module-list.csv` is `wxtnlbuffer.cpp`, so you can select it with `-M539`.

Run it under the project's Wine prefix, e.g.:

```
WINEPREFIX=$PWD/.wine wine tools/cvdump/cvdump.exe \
  -l -s -M539 path/to/ProjectG_ReleaseQA.pdb
```

The output can be a little terse. For example, this line output:

```
d:\build\custom\temp\code\client\wangreal\source\wxtnlbuffer.cpp (None), 0001:004FD580-004FD5C3, line/addr pairs = 8

  894 004FD580    898 004FD585    899 004FD590    901 004FD59F
  903 004FD5A7    906 004FD5AD    909 004FD5BF    910 004FD5C2
```

...tells us, among other things, that line 910 of `wxtnlbuffer.cpp` corresponds to the offset `0x004FD5C2` in the .text section, which we can map to a contribution with `address2sym` by adding the .text section RVA base, `0x1000`:

```
$ tools/address2sym.py --rva 004FE5C2
0x004fe5c2  (rva 0x4fe5c2)
  section 1 .text +0x4fd5c2
  module 538: \build\custom\temp\garbage\s5\client\releaseqa\wangreal\wxtnlbuffer.obj (in fresh.lib)
  path: source/client/Wangreal/source/wxtnlbuffer.obj
  contribution 45301: 0x004fe580..0x004fe5c3 (size 0x43, +0x42)
```

or, often even more useful, we can use the `0001:004FD580-004FD5C3` specifier to map back to a symbol:

```
$ WINEPREFIX=$PWD/.wine wine tools/cvdump/cvdump.exe -S research/ProjectG_ReleaseQA.pdb | grep 0001:004FD580
S_PUB32: [0001:004FD580], Flags: 00000002, ?xFillBuffers@WxTnLBuffer@@QAEHXZ
(001F90) S_GPROC32: [0001:004FD580], Cb: 00000043, Type:             0x6600, WxTnLBuffer::xFillBuffers
  d:\build\custom\temp\code\client\wangreal\source\wxtnlbuffer.cpp (None), 0001:004FD580-004FD5C3, line/addr pairs = 8
  021B  0001:004FD580  00000043  60501020
```

This can be useful to try to piece together the structure of a translation unit, although it is inherently lossy to some degree.
