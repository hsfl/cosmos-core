var sensorCount = getNodeData("device_tsen_cnt");
var sensors = new Array();
for (var i = 0; i<sensorCount; i++) {
	sensors[i] = new TemperatureMonitor(this);
	var data = new COSMOSDatumWdgt(sensors[i]);
	data.valueName = "device_tsen_temp_" + ("000"+i).slice(-3);
}
OnResize();

function OnResize() {
	var x_coord=10;
	var y_coord=10;
	for (var i = 0; i<sensors.length; i++) {
		sensors[i].geometry = QRect(x_coord, y_coord, 40, 25);
		x_coord += 50;
		if (x_coord+40 > geometry.width) {
			x_coord = 10;
			y_coord += 35;
		}
	}
}
