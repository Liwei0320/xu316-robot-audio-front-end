# DATA0 单麦硬件验证版

## 定位

本版本用于保留“只连接一个麦克风到 DATA0 时成功录到人声”的已验证状态。它不是编译期真正的单通道程序：固件仍配置 2 路 PDM 输入和 2 路 USB 输入通道。

| USB 通道 | PDM 信号 | XU316 引脚 | 处理 |
|---|---|---|---|
| 通道 1 / 左 | DATA0 | X0D40 / P8D bit 4 | 饱和放大 64 倍 |
| 通道 2 / 右 | DATA1 | X0D41 / P8D bit 5 | 饱和放大 64 倍；验证时未接麦克风 |

配置：48 kHz、16-bit、UAC1、PDM 下降沿采样、单个 PDM 抽取子任务。USB 产品名为 `XU316 Beamformer Audio Test 7`，UAC1 PID 为 `0x0327`。

## 已验证文件

| 文件 | SHA256 |
|---|---|
| `bin/app_single_mic_data0_verified_20260809.xe` | `F5DE06576EB5ED5C25ED7D53737A63ACFDF9DD945A810994D9D377EA24B6FC08` |
| `test_evidence/my_record_10s.wav` | `7078789B201D366376011EF159867EADDEAD15B182CB86A9FEC25C2E3475B9EF` |

录音为 48 kHz、16-bit、2 通道、10 秒。DATA0 通道 RMS 约 `-24.18 dBFS`、峰值约 `-3.70 dBFS`、无削波；DATA1 当时未接，第二通道仅接近 `-1～1 LSB`。

## 运行与录音

```powershell
cd C:\xmos_work\sq66_firmware_versions\variants\single_mic_hardware_validation
.\run_xtag.cmd U8NUL5P2
```

保持 `xrun` 窗口运行，在另一个终端执行：

```powershell
python .\host_tools\record_single_mic_diagnostic.py -t 10 -o C:\xmos_work\single_new.wav
```

## 源码来源和限制

- `source` 根据当日完整源码输出、构建时间链和随后转换为双麦的精确补丁恢复；详见 `../../docs/SINGLE_SOURCE_PROVENANCE.md`。
- 历史 `.xe` 是硬件验证基准；恢复源码尚未重新构建并证明能逐字节复现该固件。
- 当前程序绕过波束形成，仅作 DATA0 采集和 DATA1 空载对照。
- 仅允许 XTAG `xrun` 加载到 RAM；DFU/QSPI 均关闭，不得 `xflash`，不得将 `.xe` 交给 CH347 写入。
