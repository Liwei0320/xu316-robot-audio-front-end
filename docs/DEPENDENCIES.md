# XMOS 依赖锁定

三个版本使用同一套 XMOS 依赖。依赖目录本身原来都是独立 Git 仓库，因此本归档不直接嵌套提交，而是记录精确 commit，并保存两个有本地修改的库的补丁和完整文件副本。

## 版本表

| 目录 | Tag | 完整 commit | Origin | 本地状态 |
|---|---|---|---|---|
| `lib_adat` | `v2.0.1` | `d463daef6d0ec54951ee46f45029d0012aa2a112` | `https://github.com/xmos/lib_adat` | clean |
| `lib_locks` | `v2.4.0` | `cdfd38064c1f1af43825c17606aa0850b707cb05` | `https://github.com/xmos/lib_locks` | clean |
| `lib_logging` | `v3.4.0` | `f096e830b30fc1fa5ec58cdc237f675bd439e8f6` | `https://github.com/xmos/lib_logging` | clean |
| `lib_mic_array` | `v6.0.0` | `73d5f9e1c5c180f79948bc19ebbf10450d4e16e8` | `https://github.com/xmos/lib_mic_array` | 2 个本地修改文件 |
| `lib_spdif` | `v7.0.0` | `4b443bcbab51ca4fdbd0b670f3e2ade7d4b332c9` | `https://github.com/xmos/lib_spdif` | clean |
| `lib_sw_pll` | `v2.4.1` | `e02a4a78b787b67183837fadff4c9c8ac47d78ed` | `https://github.com/xmos/lib_sw_pll` | clean |
| `lib_xassert` | `v4.3.2` | `54964d9373bf4b5dc132e3f24b49ebe40cfdfc8f` | `https://github.com/xmos/lib_xassert` | clean |
| `lib_xcore_math` | `v2.4.0` | `9e730e0796d66df20d6af2c9356ae5fd3f0aa045` | `https://github.com/xmos/lib_xcore_math` | clean |
| `lib_xua` | `v5.3.0` | `4fba3810150e8ff9f063ca51a56e5406d6572423` | `https://github.com/xmos/lib_xua` | 6 个本地修改文件 |
| `lib_xud` | `v4.0.1` | `7683d50c9d78f820aeaebeb3c29812c39104f444` | `https://github.com/xmos/lib_xud` | clean |

## 本地补丁

| 补丁 | SHA256 | 作用 |
|---|---|---|
| `shared/dependency_patches/lib_mic_array_local_changes.patch` | `7EFFDAEC1B26855E33553D6C130124F08CBF45E0CFA7BED1B4E3822C5C9BCEDA` | PDM 资源/时钟驱动和并行抽取支持修正 |
| `shared/dependency_patches/lib_xua_local_changes.patch` | `0C3501B040C47FD114ADDCDCA91E72155B921ABED9850A463BEA16986934C587` | USB/PDM 时钟、常开采集、缓冲及并行抽取集成修正 |

两个补丁都已对上述精确基础 commit 做过反向应用检查，证明归档补丁与当时已修改的工作树一致。

补丁是恢复本地改动的首选方式。`shared/dependency_modified_files` 另外保存 8 个修改后的完整文件，仅用于补丁无法应用时人工对比或恢复。不要先应用补丁再重复覆盖完整文件。

## 完整文件副本校验

完整文件内容与当时工作树一致；归档时按 `.gitattributes` 将文本统一为 LF，因此下表记录的是仓库内规范化文件的 SHA256，而不是原 Windows CRLF 文件的字节哈希。

| 相对路径 | SHA256 |
|---|---|
| `lib_mic_array/lib_mic_array/src/mic_array_setup.c` | `DE0F71AC14861E7A7C58A5ACDDD654D54F24893F7B9E5F1F5BA7CE230A1D4C47` |
| `lib_mic_array/lib_mic_array/src/mic_array_task_internal.hpp` | `3C4B1ADA76EBA73A6176E2CADBA4043FE48A953EEBA4237901ABE66B6E1EEEB1` |
| `lib_xua/lib_xua/api/xua_conf_default.h` | `51502E5DD05896E26F92EBC890017372A707124888D0AD7AE3B189801490AFA4` |
| `lib_xua/lib_xua/src/core/audiohub/xua_audiohub.xc` | `38DF1878A611B0DB3C59EA3AB6A9406AF3135CC17D03D17982BB43787E37A4DC` |
| `lib_xua/lib_xua/src/core/buffer/decouple/decouple.xc` | `D84EFFCA714A4A84C9AA68C30ACCE0440984663C20C4045FF252A54E4CD60D81` |
| `lib_xua/lib_xua/src/core/main.xc` | `03BB038092724EF3CD6ACCE703FBAC28284B557819ECC7FEACBA7ADD9B705FF8` |
| `lib_xua/lib_xua/src/core/pdm_mics/mic_array_conf.h` | `D1862A6BA4FC55A91123A52554AD19121EEF1E6572AB314569EB965BC5B8981E` |
| `lib_xua/lib_xua/src/core/pdm_mics/mic_array_task.c` | `026CC2FE0FA388F2982E673355199A4DA517CA91B6D65CB3830EC14AFF1E45C2` |
