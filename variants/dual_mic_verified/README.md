# DATA0 + DATA1 双麦验证版

## 定位

本版本保存两只 PDM 麦克风独立采集并通过 USB 输出的已验证状态。

| USB 通道 | PDM 信号 | XU316 引脚 | 处理 |
|---|---|---|---|
| 通道 1 / 左 | DATA0 | X0D40 / P8D bit 4 | 饱和放大 64 倍 |
| 通道 2 / 右 | DATA1 | X0D41 / P8D bit 5 | 饱和放大 64 倍 |

配置：48 kHz、16-bit、UAC1、PDM 下降沿采样、单个 PDM 抽取子任务。USB 产品名为 `XU316 Beamformer Audio Test 7`，UAC1 PID 为 `0x0327`。

## 已验证文件

| 文件 | SHA256 |
|---|---|
| `bin/app_dual_mic_verified_20260809.xe` | `62DCB6C53AA765B2E8282608658D4A50D7168FD9400300BFB6194C801DEE98AC` |
| `test_evidence/my_dual_mic_10s.wav` | `658CEC1E4EDF8522B180B0C9940D9D098F22C49DC786E5EEEB7F0B53DCDBCD09` |

录音为 48 kHz、16-bit、2 通道、10 秒。两路 RMS 约 `-25.98/-26.28 dBFS`，峰值约 `-3.14/-6.12 dBFS`，两路均无削波。

## 运行与录音

```powershell
cd C:\xmos_work\sq66_firmware_versions\variants\dual_mic_verified
.\run_xtag.cmd U8NUL5P2
```

保持 `xrun` 窗口运行，在另一个终端执行：

```powershell
python .\host_tools\record_dual_mic.py -t 10 -o C:\xmos_work\dual_new.wav
```

## 限制

- `source` 是录音验证时保存的完整应用源码快照，依赖修改见 `../../docs/DEPENDENCIES.md`。
- DATA0、DATA1 是两个独立原始 PCM 通道；没有波束形成、AEC、DoA、AGC 或噪声门。
- 两路固定 64 倍增益，麦克风距离很近或声压较大时仍可能削波。
- 单麦和双麦版本的 Windows 产品名相同，不能只看设备名称判断版本；应按启动的文件名和 SHA256 确认。
- 仅允许 XTAG `xrun` 加载到 RAM；DFU/QSPI 均关闭，不得 `xflash`，不得将 `.xe` 交给 CH347 写入。
