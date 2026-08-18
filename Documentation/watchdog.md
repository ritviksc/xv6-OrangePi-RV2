# Watchdog Timer (WDT) 
===================================================
A Watchdog Timer is a hardware peripheral/circuit that can reset a computer system- mainly used embedded systems. it can detect software/hardware faults and forces a hardware reset as a result. For example, if memory gets corrupted the device may hang and the watchdog can restart the device to run again.The watchdog needs its counter periodically to be reset to prevent the device from rebooting. This operation is usually referred to as “feeding”, “kicking”, or “patting” the dog.

There are three types of watchdogs-hardware,software,and external WDTs.
 
### Hardware Watchdog
Resides outside the SoC on emebedded systems, and is part of the power management integrated circuit(PMIC). Can detect if parts of the device fail, and as an result the device may restart or power cycle.

### Software Watchdog
Resides directly on the SoC. It is the software resposibility to manage the watchdog to prevent unwanted resets/power cycles. 

### External Watchdog
Some devices may have external watchdogs connected by a GPIO, as an extra safety feature.

On the Orange Pi RV2, there are two watchdogs, one is a software WDT and the other one is a hardware WDT.

WDT snippet from Uboot log when starting the device
```
...
WDT: Started PMIC_WDT with servicing (60s timeout)
WDT: Started watchdog@D4080000 with servicing (60s timeout)
...

```

Watchdog node from device tree @dts/x1.dtsi
```
watchdog: watchdog@d4080000 {
			compatible = "ky,soc-wdt";
			clocks = <&ccu CLK_WDT>;
			resets = <&reset RESET_WDT>;
			reg = <0x0 0xd4080000 0x0 0xff>,
				<0x0 0xd4050000 0x0 0x1024>;
			interrupts = <35>;
			interrupt-parent = <&intc>;
			spa,wdt-disabled;
			status = "disabled";
		};

```

When Uboot hands control over to linux both watchdogs are paused to ensure linux boots properly and  only the SoC is handled by the linux as it is the only watchdog that has a linux driver (run ```ls /dev/watchdog*``` to confirm) and device tree description @dts/x1_orangepi-rv2.dts:
```
&watchdog {
	status = "okay";
};

```
The hardware watchdog is disabled (this could be by uboot or linux).
