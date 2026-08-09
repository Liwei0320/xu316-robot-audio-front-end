# DATA0～DATA3 四麦原始采集版

## 定位

本版本把四只 PDM 麦克风分别输出为四个 USB PCM 通道，用于验证硬件连线和后续算法输入。它不是四麦波束成形输出。

| USB 通道 | PDM 信号 | XU316 引脚 | 处理 |
|---|---|---|---|
| 通道 1 | DATA0 | X0D40 / P8D bit 4 | 饱和放大 64 倍 |
| 通道 2 | DATA1 | X0D41 / P8D bit 5 | 饱和放大 64 倍 |
| 通道 3 | DATA2 | X0D42 / P8D bit 6 | 饱和放大 64 倍 |
| 通道 4 | DATA3 | X0D43 / P8D bit 7 | 饱和放大 64 倍 |

配置：48 kHz、16-bit、UAC1、4 通道、PDM 下降沿采样、两个并行 PDM 抽取子任务。USB 产品名为 `XU316 Four Mic Audio Test 1`，UAC1 PID 为 `0x032B`。

## 已验证文件

| 文件 | SHA256 |
|---|---|
| `bin/app_four_mic_raw_64x_20260809.xe` | `DC506725094CE04DB56F3E043131C2051C711F69EF083C102CAFC9FFD5E7723C` |
| `test_evidence/my_four_mic_20s.wav` | `AB189558F0E7C1140AD8CAF74EE9C2CFC925AB68FACFF1D7E497DCEDCACDFFC4` |

录音为 48 kHz、16-bit、4 通道、20 秒，四路均有信号。该文件包含近距离敲击/强信号造成的削波，属于通道存在性和压力测试证据，不应作为最终音质样本。

## 运行与录音

```powershell
cd C:\xmos_work\sq66_firmware_versions\variants\four_mic_raw
.\run_xtag.cmd U8NUL5P2
```

保持 `xrun` 窗口运行，在另一个终端执行：

```powershell
python .\host_tools\record_four_mic.py -t 20 -o C:\xmos_work\four_new.wav
```

## 限制

- 四路只是独立原始 PCM，尚未执行延时求和波束形成，也没有 AEC、DoA、AGC 或噪声门。
- 四只麦克风在机壳中的最终几何位置和通道顺序尚未标定，不能直接据此计算波束延时。
- 64 倍固定增益在近讲或敲击时会削波；进入波束成形前建议评估降到 32 倍，并重新做噪声与灵敏度标定。
- 当前只完成 20 秒四路录音，尚未完成长时间稳定性和资源余量验证；tile 0 已使用全部 8 个硬件线程。
- 仅允许 XTAG `xrun` 加载到 RAM；DFU/QSPI 均关闭，不得 `xflash`，不得将 `.xe` 交给 CH347 写入。
