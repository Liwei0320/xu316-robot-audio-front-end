# SQ66 XU316 麦克风固件版本归档

本仓库归档了 2026-08-09 在自制 XU316 主板上测试过的单麦、双麦和四麦程序。每个版本均保存应用源码、预编译 `.xe`、Windows 录音脚本和对应测试录音。

## 重要结论

- 三个版本目前都只能通过 XTAG4 和 `xrun` 加载到 XU316 的 RAM；停止 `xrun` 或断电后程序消失。
- 当前配置关闭了 DFU 和 QSPI Flash。不要对这些 `.xe` 执行 `xflash`，也不要用 CH347 直接写入 `.xe`。
- 三个版本都是麦克风采集/硬件验证程序，不是波束成形、AEC、DoA 或量产固件。
- “单麦版”是只物理连接 DATA0 的历史验证状态，固件内部仍有两个 PDM/USB 输入通道；详细说明见该版本 README。
- 原工程中 2026-07-27 的旧 `sq66_beamformer_factory.bin` 与本仓库三个版本均不一致，已明确排除在归档之外。

## 版本一览

| 目录 | 硬件输入 | USB 输出 | 验证状态 | 推荐固件 |
|---|---|---|---|---|
| `variants/single_mic_hardware_validation` | 测试时仅接 DATA0 | 2 通道；DATA0、DATA1 各自 64 倍 | DATA0 人声录音通过；源码由历史记录高可信恢复 | `app_single_mic_data0_verified_20260809.xe` |
| `variants/dual_mic_verified` | DATA0、DATA1 | 2 个独立通道，各 64 倍 | 双麦 10 秒录音通过 | `app_dual_mic_verified_20260809.xe` |
| `variants/four_mic_raw` | DATA0～DATA3 | 4 个独立通道，各 64 倍 | 四麦 20 秒原始采集通过；近讲有削波 | `app_four_mic_raw_64x_20260809.xe` |

共同音频配置为 48 kHz、16-bit、UAC1、PDM 下降沿采样。四麦版使用两个并行 PDM 抽取子任务。

## 快速运行

先打开“XMOS XTC Tools 15.3.1 Command Prompt”，确认 XTAG4 和主板的 USB 数据线都已连接，然后执行：

```powershell
cd C:\xmos_work\sq66_firmware_versions
xrun -l
```

以当前使用的 XTAG4 适配器 `U8NUL5P2` 为例，运行四麦版本：

```powershell
.\variants\four_mic_raw\run_xtag.cmd U8NUL5P2
```

保持该窗口运行。在第二个终端安装录音依赖并录制：

```powershell
cd C:\xmos_work\sq66_firmware_versions
python -m pip install -r .\requirements.txt
python .\variants\four_mic_raw\host_tools\record_four_mic.py -t 10 -o C:\xmos_work\four_mic_new.wav
```

切换固件前先在运行 `xrun` 的窗口按 `Ctrl+C`，否则会出现 `device is in use by another process`。
录音脚本会选择第一个匹配名称的 WASAPI 输入；测试时只保留当前这一块 XU316 音频设备，避免误选同名端点。

## 目录说明

```text
variants/                  单麦、双麦、四麦的自包含应用快照
  <version>/source/        XMOS 应用源码
  <version>/bin/           对应的已归档 XE
  <version>/host_tools/    Windows WASAPI 录音脚本
  <version>/test_evidence/ 当时的测试 WAV
shared/dependency_patches/ lib_xua、lib_mic_array 本地修改补丁
shared/dependency_modified_files/ 修改后的完整文件备份
shared/project_scaffold/   重建工作区所需的最小根 CMake 文件
docs/                      依赖锁、恢复步骤和来源说明
SHA256SUMS.csv             全仓库文件校验清单
```

依赖库没有作为嵌套 Git 仓库直接提交，避免产生不可移植的 gitlink。完整依赖版本和恢复方法分别见 `docs/DEPENDENCIES.md` 与 `docs/RESTORE_AND_RUN.md`。

## 完整性与限制

- 固件和测试录音的 SHA256 均记录在各版本 README 和根目录 `SHA256SUMS.csv` 中；该清单覆盖除 `.git` 和清单自身之外的全部归档文件。
- 单麦历史 `.xe` 是硬件验证基准；其源码虽有完整历史文本和严格时间链支持，但尚未重新构建并证明能逐字节复现该 `.xe`。
- 双麦源码是测试通过时保存的完整快照；四麦源码是生成已测试四麦固件时的完整快照。
- `test_evidence` 中包含真实现场人声录音；本地归档可以保留，推送到远程或公开分享前应先检查隐私并按需移除。
- 新工具链构建的 `.xe` 可能因构建元数据不同而哈希不同，必须重新做 USB 枚举、通道映射、录音和长稳测试。
- XMOS 第三方源码继续受其原许可证约束；本仓库只记录本板应用、必要补丁和恢复信息。
