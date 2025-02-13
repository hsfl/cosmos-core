//loadUI(Resources+"ADCSdetail.ui", this);

var view = new Model3DView(this);
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
//print("starting");

function OnResize() {
	if (this.geometry.width<=this.geometry.height) { //This does nothing and it's a mystery!
		//print("current:");
		//print(rotationViewArea.layout);
		//print("horizontal");
		//print(horizontalLayout);		
		if(rotationViewArea.layout==horizontalLayout) {
			horizontalLayout.removeItem(rotationViewArea);
			verticalLayout.addLayout(rotationViewArea);
		}
	} else {
		//print("current");
		//print(rotationViewArea.layout);
		//print("vertical");
		//print(verticalLayout);
		if(rotationViewArea.layout==verticalLayout) {
			verticalLayout.removeItem(rotationViewArea);
			horizontalLayout.addLayout(rotationViewArea);
		}
	}
	//print("");
}
