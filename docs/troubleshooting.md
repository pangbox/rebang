# `fatal error LNK1169: one or more multiply defined symbols found`

You may run into some issues caused by COMDAT (common data) flags. The final linked executable loses the necessary information to correctly reconstruct section flags. This will result in duplicate symbol errors. To fix this, sections incorrectly marked `IMAGE_COMDAT_SELECT_NODUPLICATES` may need to be re-written to `IMAGE_COMDAT_SELECT_ANY`.

For example:

```shell
$ python tools/coffsym.py set-selection ./source/client/Wangreal/source/wview.obj '??1WView@@UAE@XZ' any
```

# Register allocation varies based on temporaries

Even though semantically, accessing a field via an inlined accessor is identical to just accessing the field directly, it will have an impact on how registers are allocated in MSVC.

For example,

```cc
WVideoDev* video = GetVideoDevice();
```

can be the same as

```cc
WResourceManager* resrc = m_resrcMng;
WVideoDev* video = resrc->video;
```

but

```cc
WVideoDev* video = m_resrcMng->video;
```

is _not_.

# Image mismatch due to out-of-line inlines

Even when every inline call to a function in a given unit is indeed inlined, in some cases it will still result in an out-of-line COMDAT copy. The final image is linked with `/DEBUG`, which prevents the unreferenced copy from being stripped. The linker will remove the duplicate copies of these inlined functions across all of the objects, leaving only one: whatever the first copy that was contributed was.

Because these inline functions are otherwise unreferenced, they usually don't have a name in the debug infomration. Because of that, they have a synthesized name instead, in the form of `__pg_c_*`. Because COMDAT deduplication is keyed off of the symbol name, these synthesized names no longer get deduplicated as expected once we start swapping some objects out for source code, resulting in an image mismatch.

The fix is to rename the symbol once it has been identified:

```shell
$ python tools/coffsym.py rename __pg_c_001786 '?GetVideoDevice@WView@@QBEPAVWVideoDev@@XZ'
$ python tools/coffsym.py set-selection ./source/client/ProjectG/pool.obj '?GetVideoDevice@WView@@QBEPAVWVideoDev@@XZ' any
```

The `rename` tool will rename the symbol across the entire tree. This is necessary so that external references can be correctly resolved.

To locate the original symbol, compile with the `/FAcs` flags. It will give you an assembly listing with data and code intermixed. Then, search for a match at a plausible symbol address. Since the relocated operands will be unresolved in the output, they need to be wildcarded. Another challenge is the fact that not all translation units were compiled with the same flags, so sometimes the copy that makes it into the image was built with different compiler flags than the one you're currently looking at. You may have to cycle through different compiler flags, such as `/Op` or `/Oy`.

There may be multiple chunks that match exactly. Unfortunately, it isn't really possible to be 100% sure which chunk corresponds to what you are looking at. Do not rename everything that is a perfect match; only one symbol can possibly be the *actual* match.

Putting `__declspec(dllimport)` on an inline function will actually stop the out-of-line copy from being emitted at all, but this is a dirty workaround for the problem. If plausible, it's better to try to find the original copy. In some cases, it may not be trivial.

# COMDAT emission order follows code generation order

The linker outputs sections in the order the compiler emits them, and the compiler emits inline bodies as soon as it finishes code generation for them. Therefore, restructuring a class can change the image identity.
