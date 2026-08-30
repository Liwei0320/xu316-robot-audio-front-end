# XU316 硬件资料

本目录归档项目自主设计的 2026-08-30 新板，包括嘉立创 EDA Pro 原理图与 PCB 工程、PCB/3D 渲染和实物验证照片。

## 文件清单

| 文件 | 说明 |
|---|---|
| `source/xu316-hardware-20260830.epro` | 可由嘉立创 EDA Pro 导入的工程归档；压缩包内包含 `2` 个 `.esch` 原理图页面、`1` 个 `.epcb` PCB 文件及工程元数据 |
| `renders/xu316-board-3d-render.png` | 原“3D图”：装配后的 XU316 主板 3D 效果图 |
| `renders/xu316-board-pcb-layout.png` | 原“PCB图”：XU316 主板 PCB 布局与走线预览 |
| `photos/xu316-board-bench-test.jpg` | 原“实测图”：主板通电、USB 接线和调试飞线实测状态 |
| `photos/xu316-four-microphone-system-test.jpg` | 原“测试图”：核心板、转接板与四麦线阵完整测试连接 |

## 关键硬件

- 主控：XMOS XU316（QF60 封装）。
- 启动存储：W25Q16JW，1.8 V QSPI NOR Flash。
- 麦克风接口：四路 PDM DATA0～DATA3，共享 MIC_CLK。
- 主机接口：USB Type-C，用于供电和 USB 音频。
- 调试接口：XTAG4，用于 JTAG 调试、RAM 运行与 QSPI 烧录。

## 已验证状态

- 各 1.8 V/3.3 V 电源轨正常。
- QSPI JEDEC ID 为 `EF 60 15`，QE 状态已开启。
- 10 MHz QSPI 烧录与连续回读一致。
- 移除 XTAG4 后，仅通过主板 Type-C 可完成冷启动和 USB 音频枚举。

这些图片记录了当前飞线和转接板调试状态，不代表后续版本的最终布线。修改工程前应重新执行 ERC/DRC，并重点复核 QSPI_CS、CLK、D0～D3、BOOT_SEL、RST_N、VDDIO 和 Flash 供电电压。
