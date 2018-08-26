<!-- Helper functions -->
function ModifyBGColor(id, newColor) {
	var mElement = getDocElementById(id);
	mElement.style.backgroundColor = newColor;
}
function ModifyTextColor(id, newColor) {
	var mElement = getDocElementById(id);
	mElement.style.color = newColor;
}
function ModifyBoxSize(id, newWidth, newHeight) {
	var mElement = getDocElementById(id);
	mElement.style.width = newWidth;
	mElement.style.height = newheight;
}
function ModifyBoxPosition(id, newLeft, newTop) {
	var mElement = getDocElementById(id);
	mElement.style.left = newLeft;
	mElement.style.top = newTop;
}	

<!-- Select Button -->
// Usage:
// <button id="tst1" class="buttonPush" onclick="onclick_button(this.id)">Button</button>
function onclick_button(id,colIdx)
{
	commandToServer("BUTT,"+id);
}

function buttonSetCol(id,colIdx)
{
	col    = "rgb(255, 0, 0)"; // Shown when wrong index is selected.
	colTab = ["rgb(0, 0, 0)","rgb(0, 0, 200)"];

	if (colIdx >= 0 && colIdx < colTab.length)
		col = colTab[colIdx];
	getDocElementById(id).style.backgroundColor = col;
}

function onChangeRadio(id)
{
	var idx = getDocElementById(id).value;

	commandToServer("RDO,"+id+","+idx);
}

<!-- Select Widget -->	
function onChangeSel(id)
{
	var idx = getDocElementById(id).selectedIndex;
	commandToServer("SEL,"+id+","+idx);
}

function setSelection(id, sel)
{
	getDocElementById(id).selectedIndex = sel;
}


<!-- Slider Widget -->
// Set the maximum number of sliders
var maxNumberofSliders = 20;
slObj = [];
for (var i=0; i<maxNumberofSliders; ++i)
	slObj[i] = new slider();

function slider() {

	this.valInc = function(dir, strID) {
		console.log('valInc  ' + "id" + strID)
		var val = +getDocElementById("id" + strID).value;
		var step = +getDocElementById("id" + strID).step;

		val = val + (step * +dir);
		getDocElementById("id" + strID).value = val;
		this.update(strID);
	}

	this.valChange = function(strID) {
		var val = +getDocElementById("idInp" + strID).value;
		getDocElementById("id" + strID).value = val;
		this.update(strID);  // Check min and max values.
	}

	this.update = function(strID) {
		console.log('update')
		val = getDocElementById("id" + strID).value;
		getDocElementById("idInp" + strID).value = val;
		commandToServer("SLI,sli"+strID+","+val);
	}
}

<!-- Color Map -->
function colorMap()
{
	this.colMap = [];
    this.min = 0;
    this.max = 0;
    this.dv  = 0;

    // Jet colormap
    this.jet = function(v) {
        var dv;
        var r = 1.0;
        var g = 1.0;
        var b = 1.0;

        if (v < this.min)
            v = this.min;
        if (v > this.max)
            v = this.max;
        dv = this.max - this.min;

        if (v < (this.min + 0.25 * dv))
        {
            r = 0;
            g = 4.0 * (v - this.min) / dv;
        }
        else if (v < (this.min + 0.5 * dv))
        {
            r = 0;
            b = 1.0 + 4.0 * (this.min + 0.25 * dv - v) / dv;
        }
        else if (v < (this.min + 0.75 * dv))
        {
            r = 4.0 * (v - this.min - 0.5 * dv) / dv;
            b = 0;
        }
        else
        {
            g = 1.0 + 4.0 * (this.min + 0.75 * dv - v) / dv;
            b = 0;
        }
        var col = [Math.round(r*255),Math.round(g*255),Math.round(b*255)];
        return('rgb(' + col[0] + ',' + col[1] + ',' + col[2] + ')');
    }
    
    this.getColor = function(val)
    {
    	var val = (val - this.min) / this.dv;
    	return this.colMap[ Math.round(val) ];
    }

    this.createMap = function(min, max, size)
    {
        this.min = min;
        this.max = max;

        this.dv = (this.max - this.min) / (size - 1);
        for (var i=0; i<size; ++i)
            this.colMap[i] = this.jet(this.min + i*this.dv);
    }
}

<!-- Graphing Widget -->
//graph(strCanvas) - Normal graph
//graph(strCanvas,height1,height2,height3) - Graph plus waterfall
//Waterfall, height1 plot, height2 marker, height3 waterfall
function graph(strCanvas, height1, height2, height3) {
	var axisData   = 0;	
	//var bWaterfall = 0;
	var canvas = getDocElementById(strCanvas);

	if (null == canvas || !canvas.getContext) {
		console.log('graph: Canvas "' + strCanvas + '" not found')
		return;
	}
	
	if (typeof height1 == "undefined")
		this.height1 = canvas.height;
	else
	{
		this.bWaterfall = 1;
		this.height1 = height1;
		this.height2 = height2;
		this.height3 = height3;
	}

	this.ctx = canvas.getContext("2d");
	if ((0.15 * canvas.width) > 40)
	{
		this.x1 = 40; //0.1 * canvas.width;
		this.x2 = canvas.width - 10;
	}
	else
	{
		this.x1 = 0;
		this.x2 = canvas.width;
	}

	if ((0.15 * this.height1) > 20)
	{
		this.y1 = 30; //0.2 * canvas.height;
		this.y2 = this.height1 - 10;
	}
	else
	{
		this.y1 = 0;
		this.y2 = this.height1;
	}
	this.yy = this.height1;
	
	this.calcSpacing = function(v1, v2)
	{
		var div = 0;
		var dif = v2 - v1;
		var p = Math.log10(dif);
		var k = Math.floor(p/3);

		if (k != 0)
		{
			div = Math.pow(10,k*3);
			//x1  = x1 / div;
			//x2  = x2 / div;
			dif = v2 - v1;
		    p   = Math.log10(dif);
		}

		var sp = Math.pow(10,Math.floor(p));

		if ((dif / sp) < 5)
		{
			sp /= 2;
		    if ((dif / sp) < 5)
		    	sp /= 2.5;
		}
		return [div,sp];
	}

	this.axis = function(rx1, ry1, rx2, ry2, col) {
		tmp = this.calcSpacing(rx1, rx2);
		this.xDiv = tmp[0];
		if (this.xDiv == 0)
			this.xDiv = 1;
		this.xSp = tmp[1] / this.xDiv;		
		this.rx1 = rx1 / this.xDiv, this.rx2 = rx2 / this.xDiv;

		tmp = this.calcSpacing(ry1, ry2);
		this.yDiv = tmp[0];
		if (this.yDiv == 0)
			this.yDiv = 1;
		this.ySp = tmp[1] / this.yDiv;
		this.ry1 = ry1 / this.yDiv, this.ry2 = ry2 / this.yDiv;

		this.dx = (this.x2 - this.x1) / (this.rx2 - this.rx1);
		this.dy = (this.y2 - this.y1) / (this.ry2 - this.ry1);
		
		if (this.bWaterfall == 1)
		{
			this.cMap = new colorMap();
			this.cMap.createMap(this.ry1, this.ry2, 255);
		}

		var ctx = this.ctx;
		var yy = this.yy;

		ctx.beginPath();
		ctx.strokeStyle = "black";
		ctx.moveTo(this.x1, this.yy - this.y1);
		ctx.lineTo(this.x2, this.yy - this.y1);
		ctx.lineTo(this.x2, this.yy - this.y2);
		ctx.lineTo(this.x1, this.yy - this.y2);
		ctx.lineTo(this.x1, this.yy - this.y1);
		ctx.stroke();
	}

	this.vx = function(x) {
		return this.x1 + (x - this.rx1) * this.dx;
	}

	this.vy = function(y) {
		return this.yy - (this.y1 + (y - this.ry1) * this.dy);
	}

	// dp - Digits after the decimal place
	this.markersHor = function(dp)
	{
		var yp = this.yy - this.y1;
		var ctx = this.ctx;
		var v   = this.rx1;
		var ep = 1e-12; // Epsilon to prevent rounding errors
		var x  = Math.floor((v+ep) / this.xSp) * this.xSp;

		ctx.beginPath();
		ctx.font = "12px Comic Sans MS";
		ctx.fillStyle = "black";
		ctx.textAlign = "center";
		ctx.strokeStyle = "rgb(128,128,128)";
		while (x <= this.rx2+ep) // 1e-12 For rounding errors
		{
			if (x >= this.rx1-ep) // 1e-12 For rounding errors
			{
				var xx = Math.round(this.vx(x));
			    ctx.moveTo(xx,this.yy - this.y2); //yp-5);
				ctx.lineTo(xx,yp+5);
				ctx.fillText(x.toFixed(dp), xx, yp+15); 
			}
		    v += this.xSp;
		    x = Math.floor((v+ep) / this.xSp) * this.xSp;
		}
		ctx.stroke();
	}

	// dp - Digits after the decimal place
	this.markersVer = function(dp)
	{
		var ctx = this.ctx;
		var v  = this.ry1;
		var xp = this.x1;
		var ep = 1e-12; // Epsilon to prevent rounding errors
		var y  = Math.floor((v+ep) / this.ySp) * this.ySp;

		ctx.beginPath();
		ctx.font = "12px Comic Sans MS";
		ctx.fillStyle = "black";
		ctx.textAlign = "right";
		ctx.strokeStyle = "rgb(128,128,128)";
		while (y <= this.ry2+ep) // 1e-12 For rounding errors
		{
			if (y >= this.ry1-ep) // 1e-12 For rounding errors
			{
				var yy = Math.round(this.vy(y));
			    ctx.moveTo(this.x2, yy);
				ctx.lineTo(xp-5, yy);
				ctx.fillText(y.toFixed(dp), xp-10, yy+4); 
			}
		    v += this.ySp;
		    y = Math.floor((v+ep) / this.ySp) * this.ySp;
		}
		ctx.stroke();
	}	


	this.graphFunc = function(func, color, thick) {
		var ctx = this.ctx;

		ctx.beginPath();
		ctx.lineWidth = thick;
		ctx.strokeStyle = color;

		var xinc = (this.rx2 - this.rx1) / 200;
		for (var x = this.rx1; x <= this.rx2; x += xinc) {
			y = this.vy(func(x));
			if (x == this.rx1)
				ctx.moveTo(this.vx(x), y);
			else
				ctx.lineTo(this.vx(x), y);
		}
		ctx.stroke();
	}

	this.limitY = function(y)
	{
		if (y > this.ry2)
			y = this.ry2;
		else if (y < this.ry1)
			y = this.ry1;
		return y;
	}

	this.saveAxis = function() {	
		this.axisData = this.ctx.getImageData(this.x1-3, this.yy - this.y2-3, this.x2-this.x1+6, this.y2-this.y1+6);
	}

	this.putAxis = function() {
		this.ctx.putImageData(this.axisData, this.x1-3, this.yy - this.y2-3);
	}

	this.drawMove = function(arr) {
		var x = this.x1;
		var width = this.x2 - this.x1;
		var y = this.y2 + this.height2;
		var height = this.height3;
		var ctx = this.ctx;
		var imgData = ctx.getImageData(x, y, width, height - 1);

		var x;
		var col;
		var dx = width / (arr.length - 1);
		for (var i = 0; i < width; ++i) {
			idx = Math.round(i/dx);
			col = this.cMap.getColor(arr[idx]);
			ctx.fillStyle = col;
			ctx.fillRect(x + i, y, 1, 1);
		}
		ctx.putImageData(imgData, x, y + 1);
	}	

	this.graphArr = function(color, thick, arr) {
		var ctx = this.ctx;

		if (typeof this.axisData == "undefined")
			this.saveAxis();
		else
			this.putAxis();

		ctx.beginPath();
		ctx.lineWidth = thick;
		ctx.strokeStyle = color;

		var x;
		var dx = (this.rx2 - this.rx1) / (arr.length - 1);
		dx += dx / 1e12; // To avoid rounding errors.
		for (var i = 0; i < arr.length; ++i) {
			x = this.rx1 + i * dx;
			y = this.vy( this.limitY(arr[i]) );
			if (x == this.rx1)
				ctx.moveTo(this.vx(x), y);
			else
				ctx.lineTo(this.vx(x), y);
		}

		if (this.bWaterfall == 1)
			this.drawMove(arr)
		ctx.stroke();
	}

	this.graphHist = function(color, thick, arr) {
		var ctx = this.ctx;

		ctx.beginPath();
		ctx.lineWidth = thick;
		ctx.strokeStyle = color;

		var x;
		var ymin = this.yy - this.y1;
		var dx = (this.rx2 - this.rx1) / (arr.length - 1);
		for (var i = 0; i < arr.length; ++i) {
			x = this.rx1 + i * dx;
			y = this.vy( this.limitY(arr[i]) );
			ctx.moveTo(this.vx(x), ymin);
			ctx.lineTo(this.vx(x), y);
		}
		if (this.bWaterfall == 1)
			this.drawMove(arr);
		ctx.stroke();
	}

	this.plotFunc = function(x1, y1, x1, y1, col, width, func) {
		this.axis(x1, y1, x1, y1, "rgb(128,128,128)");
		this.graphFunc(func, col, width);
	}

	this.plotArr = function(x1, y1, x1, y1, col, width, arr) {
		this.axis(x1, y1, x1, y1, "rgb(128,128,128)");
		this.graphArr(col, width, arr);
	}

	this.plotHist = function(x1, y1, x1, y1, col, width, arr) {
		this.axis(x1, y1, x1, y1, "rgb(128,128,128)");
		this.graphHist(col, width, arr);
	}
}



function testGraph(canv1,canv2) {
	var tmp  = new Array(1000);
	var grp1 = new graph(canv1,100,20,180); 
	var grp2 = new graph(canv2);

	for (var i = 0; i < 1000; ++i)
		tmp[i] = Math.sin(i / 10);

	grp1.axis(-17, -1, 15, 1, "rgb(128,128,128)");
	grp1.markersHor(0);
	grp1.markersVer(1);	
	grp1.graphArr('#2020E0', 1, tmp);	

	grp2.axis(0, -1, 65, 1, "rgb(128,128,128)");
	grp2.markersHor(0);
	grp2.markersVer(1);	
	grp2.graphArr('#E72020', 1, tmp);

	var N = 1000;
	for (var j=0; j<400; ++j)
	{
		for (var i = 0; i < N; ++i)
		{
			//tmp[i] = -1 + (i/250) % 2; 
			tmp[i] = Math.sin(((i) / N) * 2 * M_PI);
		}
		grp1.graphArr('#2020E0', 2, tmp);
	}
}

var graphArray = [];

function getDocElementById(id)
{
	var element = document.getElementById(id);

	if (element == null)
	{
		console.log('Element:'+id+' not found')
		commandToServer('U_DIV,'+id);
	}
	return element;
}

function processReceivedMessage(recvMsg)
{
	var fld = recvMsg.split(",");
	
	if (fld[0] == 'VAL')
	{
		for (var i=1; i<fld.length-1; i += 2)
			getDocElementById(fld[i]).innerHTML = fld[i+1];
	}
	else if (fld[0] == 'div')
	{
		getDocElementById(fld[1]).innerHTML = fld.slice(2, fld.length);
	}
    else if (fld[0] == 'BUTT')
    {
    	buttonSetCol(fld[1],fld[2]);
    }
    else if (fld[0] == 'SEL')
    {
    	setSelection(fld[1],fld[2]);
    }
	else if (fld[0] == 'AllVars')
	{
		AllVariables = event.data;
		showAllVariables();
	}

	else if (fld[0] == 'GRP') {
		if (fld[1] == 'NEW')
		{
			if (fld.length == 4)
			{
				// graph(strCanvas, height1, height2, height3)
				graphArray[ fld[3] ] = new graph( fld[2] );
			}
			else
			{
				// graph(strCanvas, height1, height2, height3)
				graphArray[ fld[3] ] = new graph( fld[2], parseFloat(fld[4]), parseFloat(fld[5]), parseFloat(fld[6]) );
			}
		}
		else if (fld[1] == 'AXIS')
		{
			var idx = parseInt(fld[2]);

			graphArray[ idx ].axis(parseFloat(fld[3]), parseFloat(fld[4]), parseFloat(fld[5]), parseFloat(fld[6]));
			graphArray[ idx ].markersHor( fld[7] );
			graphArray[ idx ].markersVer( fld[8] );	
		}
		else if (fld[1] == 'ARR')
		{
			var idx = parseInt(fld[2]);
			// graphArr(col, width, arr);
			graphArray[ idx ].graphArr(fld[3], fld[4], fld.slice(8, fld.length));
		}
	}
	else if (fld[0] == 'Exit')
	{
		ServerEvent.close();
		getDocElementById("connSt").innerHTML = "Closed";
	}		
}

<!-- Client Stuff -->
var ServerEvent;

<!-- Connect to server -->
if (typeof (EventSource) !== "undefined") {
	console.log('Comm: Init')
	ServerEvent = new EventSource("http://localhost:1235");
	console.log('Comm: Waiting')

	ServerEvent.onopen = function(event) {
		console.log('Comm: Connected')
		ConfirmConnected();
		//var mElement = getDocElementById("connSt");

		//mElement.innerHTML = "Connected";
	};

	ServerEvent.onmessage = function(event) {
		var recvMsg = event.data;

		//console.log(recvMsg)
		processReceivedMessage(recvMsg);
	};

	ServerEvent.onerror = function(event) {
		html = document.getElementsByTagName("BODY")[0].innerHTML;
		console.log('\n\n\n\n'+html)

		console.log('Comm: Error  ' + EventSource.readystate)

		var mElement = getDocElementById("connSt");
		mElement.innerHTML = "Error";
	};

} else {
	document.getElementById("result").innerHTML = "Sorry, your browser does not support server-sent events...";
}

function menuSel(id)
{
	msgToServer("/command/Menu," + id);
}

<!-- Send message to server -->
function msgToServer(msg) {
	window.frames['dataframe'].window.location.replace(msg);
}

function ConfirmConnected() {
	msgToServer("/command/Connected");
}

function commandToServer(str) {
	if (str == 'Exit')
	{
		ServerEvent.close();
		getDocElementById("connSt").innerHTML = "Closed";
	}			
	msgToServer("/command/" + str);
}		

window.onbeforeunload = function() {
	delete EventSource;
}

