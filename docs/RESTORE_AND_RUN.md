# 恢复、构建与运行

## 1. 直接运行已验证固件

环境要求：

- Windows
- XMOS XTC Tools 15.3.1
- XTAG4，同时连接 XTAG4 USB 和主板 USB 数据线
- 录音脚本使用 Python（验证环境为 3.12.10）及根目录 `requirements.txt`

在 XMOS XTC Tools 命令行中先确认适配器：

```powershell
cd C:\xmos_work\sq66_firmware_versions
xrun -l
```

然后选择一个版本运行。参数可以省略；有多个 XTAG 时应传入适配器 ID：

```powershell
.\variants\single_mic_hardware_validation\run_xtag.cmd U8NUL5P2
.\variants\dual_mic_verified\run_xtag.cmd U8NUL5P2
.\variants\four_mic_raw\run_xtag.cmd U8NUL5P2
.\variants\four_mic_beamformer_broadside\run_xtag.cmd U8NUL5P2
```

一次只能运行其中一个。`xrun` 窗口必须保持打开；要换版本先按 `Ctrl+C` 结束旧进程。

这些命令只是加载到 RAM，不是永久烧录。当前四版均设置 `XUA_DFU_EN=0` 和 `XUA_QUAD_SPI_FLASH=0`，没有完成 QSPI 型号、SPI spec、启动镜像及掉电恢复验证，因此不得用 `xflash` 或 CH347 写入。

## 2. 手动录音

在第二个 PowerShell 窗口执行：

```powershell
cd C:\xmos_work\sq66_firmware_versions
python -m pip install -r .\requirements.txt
```

单麦硬件验证版和双麦版均录制 2 通道：

```powershell
python .\variants\single_mic_hardware_validation\host_tools\record_single_mic_diagnostic.py -t 10 -o C:\xmos_work\single_new.wav
python .\variants\dual_mic_verified\host_tools\record_dual_mic.py -t 10 -o C:\xmos_work\dual_new.wav
```

四麦版录制 4 通道：

```powershell
python .\variants\four_mic_raw\host_tools\record_four_mic.py -t 20 -o C:\xmos_work\four_new.wav
```

四麦宽侧波束版录制 2 通道，左右都是同一个处理结果：

```powershell
python .\variants\four_mic_beamformer_broadside\host_tools\record_beamformer.py -t 20 -o C:\xmos_work\beamformer_new.wav
```

前三个历史脚本选择 Windows WASAPI 下名称含 `XU316` 或 `Beamformer` 的第一个合格输入；波束版脚本只匹配名称含 `4Mic Broadside Beam` 的 2 通道输入。脚本最多等待 30 秒。录音时只保留当前这一块 XU316 音频设备，确认旧的 `xrun` 已退出，避免误选另一块同名设备。若没有找到设备，先检查 `xrun` 是否仍在运行、主板 Type-C 是否为数据线、以及 Windows 设备管理器是否出现 USB Audio 输入端点。

## 3. 重建源码工作区

建议在仓库外新建工作目录。以下示例使用 `C:\xmos_work\sq66_rebuild`：

```powershell
$archive = 'C:\xmos_work\sq66_firmware_versions'
$work = 'C:\xmos_work\sq66_rebuild'
New-Item -ItemType Directory -Path $work -Force
Copy-Item "$archive\shared\project_scaffold\CMakeLists.txt" $work
Copy-Item "$archive\variants\dual_mic_verified\source\app_sq66_beamformer" $work -Recurse
```

将示例中的 `dual_mic_verified` 换成另一个版本目录即可重建对应应用。工作目录最终应为：

```text
sq66_rebuild/
  CMakeLists.txt
  app_sq66_beamformer/
  lib_adat/
  lib_locks/
  lib_logging/
  lib_mic_array/
  lib_spdif/
  lib_sw_pll/
  lib_xassert/
  lib_xcore_math/
  lib_xua/
  lib_xud/
```

根据 `docs/DEPENDENCIES.md` 克隆每个库并检出表中完整 commit。例如：

```powershell
git clone https://github.com/xmos/lib_xua "$work\lib_xua"
git -C "$work\lib_xua" checkout 4fba3810150e8ff9f063ca51a56e5406d6572423
git clone https://github.com/xmos/lib_mic_array "$work\lib_mic_array"
git -C "$work\lib_mic_array" checkout 73d5f9e1c5c180f79948bc19ebbf10450d4e16e8
```

其余 8 个库按依赖表采用相同步骤。所有库检出完成后，只应用两份本地补丁：

```powershell
git -C "$work\lib_xua" apply "$archive\shared\dependency_patches\lib_xua_local_changes.patch"
git -C "$work\lib_mic_array" apply "$archive\shared\dependency_patches\lib_mic_array_local_changes.patch"
```

在 XMOS XTC Tools 15.3.1 Command Prompt 中配置和构建：

```powershell
cd C:\xmos_work\sq66_rebuild
cmake -G "Unix Makefiles" -S . -B build
xmake -C build -j
```

生成文件位于 `app_sq66_beamformer\bin\app_sq66_beamformer.xe`。

## 4. 重建后的验收要求

- `cmake`、`xmake` 无错误，且资源报告没有线程或内存溢出。
- 通过 XTAG `xrun` 启动后，Windows 能以预期产品名和通道数枚举。
- 对原始采集版逐个靠近或轻敲麦克风，确认 DATA0～DATA3 与 USB 通道映射正确。
- 对波束版先确认左右输出基本一致，再分别录制正面和侧面人声，检查有效声音、底噪、削波和方向性；真实麦克风坐标未标定前不能宣称已转向具体角度。
- 保存 48 kHz、16-bit WAV，按各版本通道定义检查人声、底噪和削波。
- 至少进行一次长时间连续录音测试，再决定是否把新构建标为“已验证”。

新的 `.xe` 可能因路径、工具链或构建元数据不同而无法复现归档固件的 SHA256；不能仅凭哈希不同判断功能源码不同。
