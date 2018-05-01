var Accessory = require('../').Accessory;
var Service = require('../').Service;
var Characteristic = require('../').Characteristic;
var uuid = require('../').uuid;
var mqtt = require('mqtt');

var currentState = 0;

var stateless = false;

//change this//
var name = "Button2";
var UUID = "hap-nodejs:accessories:sw2";
var USERNAME = "5C:8D:17:73:1C:A4";
var MQTT_IP = '192.168.0.56'
var lightTopic = 'Button2'
//change this//

//MQTT Setup
var options = {
	port: 1883,
	host: MQTT_IP,
	clientId: 'button2'
};
var client = mqtt.connect(options);
client.subscribe(lightTopic);
client.on('message', function(topic, message) {
//	console.log("New MQTT:");
	currentState = parseInt(message);
	console.log(currentState);
});
//setup HomeKit switch object
var switchUUID = uuid.generate(UUID);
var vSwitch = exports.accessory = new Accessory(name, switchUUID);
//Add properties for publishing
vSwitch.username = USERNAME;
vSwitch.pincode = "031-45-154";
//Add a light service ant setup the on characterisitc
vSwitch
	.addService(Service.Switch)
	.getCharacteristic(Characteristic.On)
	.on('get', function(callback){
	callback(null, switchAction.getState());
	});
	vSwitch
	.getService(Service.Switch)
	.getCharacteristic(Characteristic.On)
	.on('set', function(value, callback){
		switchAction.setState(value);
		callback();
	});
var switchAction = {
	//initialize the local state variable
	currentState: 0,
	//On Characteristic set/get
	getState: function() { return this.currentState;},
	setState: function(newState){
		if(newState!= this.currentState){  //only update variables and mqtt if the newStateand currentState are different
			if(newState){client.publish(lightTopic, '1');
			client.publish('MandoIR', 'boton2on');
			}
			else{client.publish(lightTopic, '0');
			client.publish('MandoIR', 'boton2off');
			}
			this.currentState = newState;
		}
	},
	update: function(){  //update the local currentState witch mqtt currentState
		this.setState(currentState);
	}
}
// update the characterisitc values so interested iOS devices can get notified
setInterval(function(){
	switchAction.update();
	vSwitch
		.getService(Service.Switch)
		.setCharacteristic(Characteristic.On, switchAction.getState());
		if(stateless == true && switchAction.getState() == true){
			switchAction.setState(false);
		}
}, 2000);
