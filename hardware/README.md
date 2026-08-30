# XU316 硬件资料

本目录归档 2026-08-30 新板对应的嘉立创 EDA Pro 工程、PCB/3D 渲染和实物验证照片。

## 文件清单

| 文件 | 说明 |
|---|---|
| `source/xu316-hardware-20260830.epro` | 可由嘉立创 EDA Pro 导入的工程归档；压缩包内包含 `2` 个 `.esch` 原理图页面、`1` 个 `.epcb` PCB 文件及工程元数据 |
| `renders/xu316-board-3d-top.png` | 装配后的主板 3D 顶视图 |
| `renders/xu316-pcb-layout-top.png` | PCB 顶层布局与走线预览 |
| `photos/xu316-core-prototype.jpg` | XU316 核心板实物和调试飞线 |
| `photos/four-mic-test-setup.jpg` | 核心板、转接板与四麦线阵连接全景 |

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

这些图片包含原型阶段飞线和转接板，不代表最终量产布线。修改工程前应重新执行 ERC/DRC，并重点复核 QSPI_CS、CLK、D0～D3、BOOT_SEL、RST_N、VDDIO 和 Flash 供电电压。
