var root = loadUI(Resources+"ADCSsummary.ui", this);

root.RotationView1.linkTo(root.RotationView0);
root.RotationView2.linkTo(root.RotationView0);

var frameSelector = root.findChildren("RefFrameSelector");
var rotationViews = root.findChildren().filterByType("RotationView");
var model3DViews = root.findChildren().filterByType("Model3DView");
if (frameSelector.length>0) {
	frameSelector[0].addItem("ICRF");
	frameSelector[0].addItem("TOPO");
	frameSelector[0].addItem("GEOC");
	frameSelector[0].addItem("LVLH");	
	frameSelector[0]["currentIndexChanged(int)"].connect(switchFrame);
	frameSelector[0].setCurrentIndex(3);
}

function switchFrame(frame) {
	var postfix = ["_s", "_v", "_a"];
	switch(frame) {
	case 0:
		for (var i = 0; i<rotationViews.length; i++) rotationViews[i].cosmosDatum.valueName = "node_loc_att_icrf"+postfix[i];
		for (i = 0; i<model3DViews.length; i++) model3DViews[i].mode = 2;
		break;
	case 1:
		for (var i = 0; i<rotationViews.length; i++) rotationViews[i].cosmosDatum.valueName = "node_loc_att_topo"+postfix[i];
		for (i = 0; i<model3DViews.length; i++) model3DViews[i].mode = 1;
		break;
	case 2:
		for (var i = 0; i<rotationViews.length; i++) rotationViews[i].cosmosDatum.valueName = "node_loc_att_geoc"+postfix[i];
		for (i = 0; i<model3DViews.length; i++) model3DViews[i].mode = 4;
		break;
	case 3:
		for (var i = 0; i<rotationViews.length; i++) rotationViews[i].cosmosDatum.valueName = "node_loc_att_lvlh"+postfix[i];
		for (i = 0; i<model3DViews.length; i++) model3DViews[i].mode = 0;
		break;
	}
}

//Still working on the following, do not delete -Erik Wessel
/*var view = new Model3DView(this);
var attitude = new RotationView(this);
new COSMOSDatumWdgt(attitude);
attitude.cosmosDatum.valueName = "node_loc_att_lvlh_s";
var rate = new RotationView(this);
new COSMOSDatumWdgt(rate);
rate.cosmosDatum.valueName = "node_loc_att_lvlh_v";
var acceleration = new RotationView(this);
new COSMOSDatumWdgt(acceleration);
acceleration.cosmosDatum.valueName = "node_loc_att_lvlh_a";
rate.linkTo(attitude);
acceleration.linkTo(attitude);
attitude.eulerHLabel = "Yaw";
attitude.eulerELabel = "Pitch";
attitude.eulerBLabel = "Roll";
rate.showLabels = false;
acceleration.showLabels = false;
var rotationViewArea = new QHBoxLayout();
rotationViewArea.addWidget(attitude);
rotationViewArea.addWidget(rate);
rotationViewArea.addWidget(acceleration);
var verticalLayout = new QVBoxLayout(this);
var horizontalLayout = new QHBoxLayout();
horizontalLayout.addWidget(view);
verticalLayout.addLayout(horizontalLayout);
verticalLayout.addLayout(rotationViewArea);

function OnResize() {
	if (geometry.width<=geometry.height) {
		if(rotationViewArea.layout==horizontalLayout) {
			horizontalLayout.removeItem(rotationViewArea);
			verticalLayout.addLayout(rotationViewArea);
		}
	} else {
		if(rotationViewArea.layout==verticalLayout) {
			verticalLayout.removeItem(rotationViewArea);
			horizontalLayout.addLayout(rotationViewArea);
		}
	}
}*/
