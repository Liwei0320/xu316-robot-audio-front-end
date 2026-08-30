# DATA0-DATA3 四麦宽侧波束成形版

## 定位与状态

本版本在 XU316 上执行固定时域 delay-and-sum 波束成形。四路 PDM 麦克风先去直流、校准和延时，再等权平均为一路处理音频，并复制到 USB 左右声道。

2026-08-10 已完成 XTAG4 RAM 加载、Windows USB 枚举和 20 秒连续静音基线录音。固件已进入波束算法分支且 USB 传输持续运行，但静音录音不能证明四路麦克风都正确参与；还需要逐麦激励和正面/侧面标准声源测试，因此不能把本版本称为已经转向某个具体角度的成熟定向波束固件。

## 信号链

```text
DATA0-DATA3
  -> 每路 70 Hz 一阶 DC blocker
  -> 每路 Q15 灵敏度校准（当前均为 1.0x）
  -> Q8 分数延时（当前四路均为 0 sample）
  -> 四路等权相加并除以 4
  -> 公共增益 16.0x
  -> USB 左右声道复制同一个处理结果
```

| 逻辑麦克风 | 主板信号 | XU316 引脚 | 当前延时 |
|---|---|---|---|
| 麦克风 0 | DATA0 | X0D40 / P8D bit 4 | 0 sample |
| 麦克风 1 | DATA1 | X0D41 / P8D bit 5 | 0 sample |
| 麦克风 2 | DATA2 | X0D42 / P8D bit 6 | 0 sample |
| 麦克风 3 | DATA3 | X0D43 / P8D bit 7 | 0 sample |

四路全为零延时会增强垂直于麦克风阵列平面的共同声源，属于宽侧（broadside）波束。实际波束宽度和旁瓣仍取决于四个声孔的位置、间距、机壳及安装误差。

首版设置 `BF_ENABLE_POST_PROCESS=0`，没有启用尚未验证的 AGC、噪声门和硬限幅；也没有 AEC 或 DoA。这样可以先单独验证 delay-and-sum 本身，避免动态处理掩盖硬件或延时问题。

## USB 配置

| 项目 | 值 |
|---|---|
| 采样格式 | 48 kHz、16-bit、UAC1 |
| 输入端点 | 2 通道；左、右均为同一波束结果 |
| USB 设备名称 | `XU316 4Mic Broadside Beam 1` |
| VID / PID | `0x20B1 / 0x032D` |
| PDM 输入 | DATA0-DATA3，共 4 路 |
| PDM 抽取 | 2 个并行子任务 |

新的 PID 用于避免 Windows 复用原四通道采集版的 USB 描述符缓存。

## 已验证文件

| 文件 | SHA256 |
|---|---|
| `bin/app_four_mic_beamformer_broadside_20260810.xe` | `53F09E35A87F40758EBC31936B96D8A52B21E489D104786508F7BC72D459AF7F` |
| `test_evidence/beamformer_silence_20s_20260810.wav` | `426A1DCB8178941571DBCCD5A92D3F5D626164D04F6F8F278EB8F58E17F2BE4E` |

固件资源检查通过：tile 0 程序和栈上界合计约 59,904 字节，tile 1 约 6,364 字节；内存充足。tile 0 的 8 个硬件线程已全部使用，不能继续在 tile 0 新增独立任务。

静音基线录音为 48 kHz、16-bit、2 通道、20 秒，共 960,000 帧；RMS 约 `-65.65 dBFS`，峰值约 `-34.31 dBFS`，没有满幅削波。固件给左右通道写入同一数值；经 Windows WASAPI 共享模式录制后左右最大仅相差 2 个 16-bit LSB。该文件用于证明枚举、连续传输和底噪状态，不是人声或方向性验证样本。

## 运行与录音

在 XMOS XTC Tools 15.3.1 命令行中运行：

```powershell
cd C:\xmos_work\sq66_firmware_versions\variants\four_mic_beamformer_broadside
.\run_xtag.cmd U8NUL5P2
```

保持 `xrun` 窗口运行，在第二个 PowerShell 窗口录音：

```powershell
python .\host_tools\record_beamformer.py -t 20 -o C:\xmos_work\beamformer_new.wav
```

录音脚本只选择 Windows WASAPI 下名称包含 `4Mic Broadside Beam` 的 2 通道输入，避免误选旧固件或其他麦克风。

## 修改指向角度

`tools/mic_geometry.json` 中的四个坐标目前都是占位值。先以毫米为单位测量四个麦克风声孔中心的 `[x, y, z]` 坐标，再运行：

```powershell
python .\tools\calc_delays.py --azimuth 0 --elevation 90
```

把输出的 `BF_DELAY_Q8_0` 到 `BF_DELAY_Q8_3` 写入 `source/app_sq66_beamformer/src/beamformer_config.h` 并重新构建。坐标仍全为零时，工具会明确报警，不能据此宣称已完成具体角度转向。

## 限制与安全

- 当前只确认了包含零延时求和代码的固件能够运行、USB 能枚举并连续录制 20 秒静音基线；还需逐麦激励证明四路均参与，并完成正面人声、侧面人声、灵敏度、削波及长时间稳定性测试。
- 公共增益当前为 16 倍。若正常说话音量过小，应先测 WAV 峰值再逐步调整，不能直接恢复每路 64 倍，以免单路在求和前不可逆削波。
- 本版本没有自适应干扰抑制、混响消除、AEC、DoA、自动转向、AGC 或噪声门。
- 仅允许通过 XTAG4 和 `xrun` 加载到 RAM；停止 `xrun` 或断电后程序消失。
- DFU 和 QSPI 均关闭，不得执行 `xflash`，也不得把 `.xe` 交给 CH347 直接写入。
