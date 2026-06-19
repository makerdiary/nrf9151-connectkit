# Revision history

## Hardware

### Rev.A1 <small>June 18, 2026</small> { id="REVA1" }

- Updated to nRF9151 A1 Module with NTN Support.
- No changes to schematics or PCB.

### Rev.A0 <small>December 25, 2024</small> { id="REVA0" }

- The first public release.

## Software & Documentation

### V2.0.0 <small>June 18, 2026</small> { id="DOC_V2.0.0" }

- Uses NCS `v3.3.99-95ed8f7e7406`.
- Refactored Interface MCU firmware to improve stability and reduce memory usage.
- Updated nRF9151 modem firmware to `v2.0.4`, adding support for A0 and A1 revision.
- Removed `serial_lte_modem` application. Will be replaced with [`ncs-serial-modem`](https://github.com/nrfconnect/ncs-serial-modem).
- Removed `nrf_cloud_rest_device_message` sample.
- Removed `nrf_cloud_multi_service` sample.
- Added `nrf_cloud_coap_cell_location` sample.
- Added `nrf_cloud_coap_device_message` sample.
- Added `nrf_cloud_mqtt_cell_location` sample.
- Added `nrf_cloud_mqtt_device_message` sample.
- Added `system_off` sample for quiescent power consumption measurement.
- Updated documentation accordingly.

### V1.0.0 <small>Mar 7, 2025</small> { id="DOC_V1.0.0" }

- The first public release.
