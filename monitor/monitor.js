(function() {
	var getTime=function() {
		return new Date()['getTime']();
	};
	var loadingScreen=(function() {
		var container;
		var loaded=false;
		var dragenter=function() {
			var e=d3['event'];
			e['stopPropagation']();
			e['preventDefault']();
			onError('');
		};
		var dragover=function() {
			var e=d3['event'];
			e['stopPropagation']();
			e['preventDefault']();
		};
		var drop=function() {
			var e=d3['event'];
			e['stopPropagation']();
			e['preventDefault']();
			if (loaded) {
				return ;
			}
			handleFiles(e['dataTransfer']['files']);
		};
		var fileReader=new FileReader;
		var fileName;
		var txtError;
		var iStatus;
		var setLoadingStatus=function() {
			txtError['text']('');
			iStatus['classed']({'fa-file-text-o':false,'fa-spinner':true,'fa-pulse':true});
		};
		var setWaitingStatus=function() {
			iStatus['classed']({'fa-file-text-o':true,'fa-spinner':false,'fa-pulse':false});
		};
		var parseCalculationFile=function() {
			parseBase64();
			document['title']=data['control']['title'];
		};
		var parseExperimentData=function() {
			document['title']=fileName;
			parseMatlab();
		};
		var tryParse=function(callback) {
			if (callback===parseCalculationFile) {
				return false;
			}
			//try {
				callback();
				loaded=true;
				return true;
			//}
			//catch (e) {
				return false;
			//}
		};
		var onSuccess=function() {
			if (!tryParse(parseCalculationFile)
				&&!tryParse(parseExperimentData)
				) {
				onError('Runtime Error.');
				return ;
			}
			maxPeople=data['playground']['maxPeople'];
			totalFrame=data['frame']['total'];
			container['classed']('invisible',true);
			rendering.init();
			initControl();
			Ruler.init();
			onWindowResize();
			renderFrame(0);
		};
		var onError=function(msg) {
			txtError['text'](msg);
			setWaitingStatus();
		};
		var onFileLoaded=function() {
			try {
				data=JSON['parse'](fileReader.result);
			}
			catch (e) {
				onError("Syntax Error");
			}
			onSuccess();
		};
		var handleFiles=function(files) {
			var file=files[0];
			fileName=file['name'];
			if (!/\.json$/['test'](fileName)) {
				onError('File Type Error');
				return ;
			}
			setLoadingStatus();
			fileReader['onload']=onFileLoaded;
			fileReader['readAsText'](file);
		};
		var init=function() {
			var body=d3['select']('body');
			body['on']('dragenter',dragenter,false);
			body['on']('dragover',dragover,false);
			body['on']('drop',drop,false);
			container=body['append']('div');
			container['classed']('loading',true);
			var txtInstruction=container['append']('p');
			txtInstruction['text']('Drop JSON here.');
			txtError=container['append']('p')['classed']('error',true);
			iStatus=container['append']('p')['append']('i')['classed']('fa',true);
			setWaitingStatus();
		};
		return {//loadingScreen
			init:init
		};
	})();
	var data;
	var loadJsonData=(function() {
		return function() {
			d3['json'](
				'./out.trajectory.json',
				function(a) {
				}
			);
		};
	})();
	var parseBase64=(function() {
		var parseByte=function(s) {
			return base64js['toByteArray'](s);
		};
		var parseFloat64=function(s) {
			var a=base64js['toByteArray'](s);
			return new Float64Array(a['buffer']);
		}
		return function() {
			data['frames']['forEach'](function (frame) {
				frame['exist']=parseByte(frame['exist']);
				frame['position']=parseFloat64(frame['position']);
				frame['velocity']=parseFloat64(frame['velocity']);
				frame['acceleration']=parseFloat64(frame['acceleration']);
				frame['destGate']=parseByte(frame['destGate']);
			});
		}
	})();
	var parseMatlab=(function() {
		var timeStep=0.1;
		var mat;
		var findRange=function(i,unit) {
			var min,max;
			mat['forEach'](function(a) {
				var x=a[i];
				if (unit!==undefined) {
					x=Math['round'](x/unit)*unit;
				}
				if (min===undefined||x<min) {
					min=x;
				}
				if (max===undefined||x>max) {
					max=x;
				}
			});
			return [min,max];
		};
		var fixByRange=function(x,range,unit) {
			x-=range[0];
			if (unit!==undefined) {
				x=Math['round'](x/unit);
			}
			return x;
		};
		return function() {
			mat=data[0];
			var tRange=findRange(0,timeStep);
			var pRange=findRange(1,1);
			var xRange=findRange(2);
			var yRange=findRange(3);
			var cRange=findRange(4,1);
			var nPeople=Math['round'](pRange[1]-pRange[0]);
			var nFrame=Math['round']((tRange[1]-tRange[0])/timeStep+1);
			data={
				"frame":{
					"total":nFrame,
					"step":timeStep
				},
				"playground":{
					"width":xRange[1]-xRange[0],
					"height":yRange[1]-yRange[0],
					"maxPeople":pRange[1]-pRange[0]
				},
				"wall":{
					"segments":[
					]
				},
				"gates":{
					"sources":[
					],
					"sinks":[
					]
				},
				"frames":[
				]
			};
			var i;
			for (i=0;i<nFrame;i++) {
				data["frames"]['push']({
					"exist":new Uint8Array(nPeople),
					"destGate":new Uint8Array(nPeople),
					"position":new Float64Array(nPeople*2),
					"velocity":new Float64Array(nPeople*2)
				});
			}
			var t,p,x,y,c,vX,vY;
			var cur;
			mat['forEach'](function(a) {
				t=fixByRange(a[0],tRange,timeStep);
				p=fixByRange(a[1],pRange,1);
				x=fixByRange(a[2],xRange);
				y=fixByRange(a[3],yRange);
				c=0;//fixByRange(a[4],cRange,1);
				vX=fixByRange(a[5],[0]);
				vY=fixByRange(a[6],[0]);
				cur=data["frames"][t];
				cur["exist"][p]=1;
				cur["destGate"][p]=c;
				cur["position"][p*2+0]=x;
				cur["position"][p*2+1]=y;
				cur["velocity"][p*2+0]=vX;
				cur["velocity"][p*2+1]=vY;
			});
		};
	})();
	var maxPeople,totalFrame;
	var backgroundColor='efefef';
	var peopleColor=['0000ff','ff0000'];
	var svgRendering=(function() {
		var svg;
		var container;
		var playgroundWidth,playgroundHeight;
		var people=[];
		var resize=function(windowWidth,windowHeight) {
			if (container===undefined) {
				return ;
			}
			container['attr']('width',windowWidth);
			container['attr']('height',windowHeight);
			var sW=windowWidth/playgroundWidth;
			var sH=windowHeight/playgroundHeight;
			if (sW<sH) {
				svg['attr']('transform','matrix('+sW+',0,0,-'+sW+','+0+','+windowHeight*(1+sW/sH)/2+')');
			}
			else {
				svg['attr']('transform','matrix('+sH+',0,0,-'+sH+','+windowWidth*(1-sH/sW)/2+','+windowHeight+')');
			}
		};
		var init=function() {
			container=d3['select']('body')['append']('svg');
			svg=container['append']('g');
			playgroundWidth=data['playground']['width'];
			playgroundHeight=data['playground']['height'];
			var i;
			var person;
			for (i=0;i<maxPeople;i++) {
				person=svg['append']('circle');
				person['attr']('r',0.2);
				people['push'](person);
			}
		};
		var calc=function(personId,visible,x,y,groupId) {
			if (visible===0) {
				people[personId]['attr']('display','none');
				return;
			}
			people[personId]['attr']('display','inline');
			people[personId]['attr']('fill','#'+peopleColor[groupId]);
			people[personId]['attr']('transform','translate('+x/visible+','+y/visible+')');
			people[personId]['attr']('opacity',visible);
		};
		var render=function() {
		};
		return {//svgRendering
			init:init,
			calc:calc,
			resize:resize,
			render:render
		};
	})();
	var threeRendering=(function() {
		var canvas;
		var scene;
		var camera;
		var renderer;
		var playgroundWidth,playgroundHeight;
		var people=[];
		var peopleGeometry=new THREE['CircleGeometry'](0.2);
		var peopleMaterial=peopleColor['map'](function(x) {
			return new THREE['LineBasicMaterial']({'color':parseInt(x,16)});
		});
		var resize=function(windowWidth,windowHeight) {
			if (renderer===undefined) {
				return ;
			}
			renderer['setSize'](windowWidth,windowHeight);
			var sW=windowWidth/playgroundWidth;
			var sH=windowHeight/playgroundHeight;
			var sM=sW<sH?sW:sH;
			camera['left']=0;
			camera['right']=playgroundWidth;
			camera['bottom']=0;
			camera['top']=playgroundHeight;
			if (sW<sH) {
				camera['bottom']-=(windowHeight/windowWidth*playgroundWidth-playgroundHeight)/2
				camera['top']+=(windowHeight/windowWidth*playgroundWidth-playgroundHeight)/2
			}
			else {
				camera['left']-=(windowWidth/windowHeight*playgroundHeight-playgroundWidth)/2
				camera['right']+=(windowWidth/windowHeight*playgroundHeight-playgroundWidth)/2
			}
			camera['updateProjectionMatrix']();
		};
		var init=function() {
			renderer=new THREE['WebGLRenderer']();
			renderer['setClearColor'](0xffffff);
			document['body']['appendChild'](renderer['domElement']);
			canvas=d3['select']('canvas');
			camera=new THREE['OrthographicCamera'](-1,1,1,-1,0.1,1000);
			scene=new THREE['Scene']();
			camera['position']['z']=10;
			camera['up']['set'](0,1,0);
			playgroundWidth=data['playground']['width'];
			playgroundHeight=data['playground']['height'];
			var background=new THREE['Mesh'](new THREE['BoxGeometry'](playgroundWidth,playgroundHeight,0.1),new THREE['LineBasicMaterial']({'color':parseInt(backgroundColor,16)}));
			background['position']['set'](playgroundWidth/2,playgroundHeight/2,-10);
			scene['add'](background);
			var i;
			var person;
			for (i=0;i<maxPeople;i++) {
				person=new THREE['Mesh'](peopleGeometry,peopleMaterial[0]);
				scene['add'](person);
				people['push'](person);
			}
			drawStaticObjects();
		};
		var drawGates=function() {
			var aSource=data['gates']['sources'];
			var aSink=data['gates']['sinks'];
			var geometry=new THREE['CircleGeometry'](1,36);
			var material;
			var object;
			var i;
			for (i=0;i<aSource['length'];i++) {
				material=new THREE['LineBasicMaterial']({'color':new THREE['Color']('#'+peopleColor[i]).lerp(new THREE['Color']('#'+backgroundColor),0.7)});
				object=new THREE['Mesh'](geometry,material);
				object['position']['set'](aSource[i][0],aSource[i][1],-2);
				object['scale']['set'](aSource[i][2],aSource[i][2],aSource[i][2]);
				scene['add'](object);
			}
			for (i=0;i<aSink['length'];i++) {
				material=new THREE['LineBasicMaterial']({'color':new THREE['Color']('#'+peopleColor[i]).lerp(new THREE['Color']('#'+backgroundColor),0.7)});
				object=new THREE['Mesh'](geometry,material);
				object['position']['set'](aSink[i][0],aSink[i][1],-2);
				object['scale']['set'](aSink[i][2],aSink[i][2],aSink[i][2]);
				scene['add'](object);
			}
		};
		var drawWalls=function() {
			var aWall=data['wall']['segments'];
			var nWall=aWall['length'];
			var geometry;
			var material=new THREE['LineBasicMaterial']({'color':0x000000});
			var i;
			for (i=0;i<nWall;i++) {
				geometry=new THREE['Geometry']();
				geometry['vertices']['push'](
					new THREE['Vector3'](aWall[i][0],aWall[i][1],-1),
					new THREE['Vector3'](aWall[i][2],aWall[i][3],-1)
				);
				scene['add'](new THREE['Line'](geometry,material));
			}
		};
		var drawStaticObjects=function() {
			drawWalls();
			drawGates();
		};
		var calc=function(personId,visible,x,y,groupId) {
			var person=people[personId];
			if (visible===0) {
				person['visible']=false;
				return ;
			}
			person['visible']=true;
			person['scale']['set'](visible,visible,visible);
			person['position']['x']=x/visible;
			person['position']['y']=y/visible;
			person['material']=peopleMaterial[groupId];
		};
		var render=function() {
			renderer['render'](scene,camera);
		};
		return {//threeRendering
			init:init,
			calc:calc,
			resize:resize,
			render:render
		};
	})();
	var rendering=threeRendering;
	var renderFrame=function(iFrame) {
		var i1=Math['floor'](iFrame);
		var i2=i1+1;
		var a1=i2-iFrame;
		var a2=1-a1;
		var i;
		var f1=data['frames'][i1];
		var f2=data['frames'][i2];
		var x,y,z;
		var gateId;
		for (i=0;i<maxPeople;i++) {
			x=0;
			y=0;
			z=0;
			if (a1>0&&f1['exist'][i]) {
				x+=f1['position'][i*2  ]*a1;
				y+=f1['position'][i*2+1]*a1;
				gateId=f1['destGate'][i];
				z+=a1;
			}
			if (a2>0&&f2['exist'][i]) {
				x+=f2['position'][i*2  ]*a2;
				y+=f2['position'][i*2+1]*a2;
				gateId=f2['destGate'][i];
				z+=a2;
			}
			rendering.calc(i,z,x,y,gateId);
		}
		rendering.render();
	};
	var createDraggable=function() {
		var obj=d3['select']('body')['append']('span');
		var xObj=0,yObj=0;
		obj['classed']('movable',true);
		obj['style']({'position':'fixed','top':xObj,'left':yObj});
		var dragging=false;
		var xLast,yLast;
		var beginMove=function() {
			var e=d3['event']['target'];
			var s;
			while (e!=obj[0]) {
				s=d3['select'](e);
				if (s['classed']('unmovable')) {
					return ;
				}
				if (s['classed']('movable')) {
					break;
				}
				e=e['parentNode'];
			}
			dragging=true;
		};
		var endMove=function() {
			dragging=false;
		}
		var updateMove=function() {
			var event=d3['event'];
			var xCurrent=event['clientX'];
			var yCurrent=event['clientY'];
			if (dragging===true) {
				xObj+=xCurrent-xLast;
				yObj+=yCurrent-yLast;
				obj['style']('left',xObj+'px');
				obj['style']('top',yObj+'px');
			}
			xLast=xCurrent;
			yLast=yCurrent;

		};
		obj['on']('mousedown',function() {
			updateMove();
			beginMove();
		});
		obj['on']('mousemove',function() {
			updateMove();
		});
		obj['on']('mouseup',function() {
			updateMove();
			endMove();
		});
		return obj;
	};
	var initControl=(function() {
		var container;
		var btnLeftFrame,btnRightFrame;
		var btnForward,btnBackward;
		var btnSpeedUp,btnSpeedDown;
		var btnPlay;
		var spanSpeedPower,iconSpeedDirection,txtTime;
		var iconPlay;
		var currentFrameF=0;
		var lastTime;
		var playing=false;
		var timeStep;
		var speed=1;
		var speedPower=0;
		var unshiftChars=function(x,n,c) {
			var ret=x+'';
			while (ret['length']<n) {
				ret=c+ret;
			}
			return ret;
		};
		var pushChars=function(x,n,c) {
			var ret=x+'';
			while (ret['length']<n) {
				ret=ret+c;
			}
			return ret;
		};
		var timeFormat1=function(t,l) {
			t=Math['round'](t);
			var ms=t%1000;
			t=Math['floor'](t/1000);
			var s=t%60;
			t=Math['floor'](t/60);
			var m=t%1000;
			t=Math['floor'](t/60);
			var h=t;
			var ret='';
			if (l>0) {
				ret=unshiftChars(s,2,'0')+'.'+unshiftChars(ms,3,'0');
			}
			if (l>1) {
				ret=unshiftChars(m,2,'0')+':'+ret;
			}
			if (l>2) {
				ret=unshiftChars(h,2,'0')+':'+ret;
			}
			return ret;
		};
		var timeFormat2=function(t1,t2) {
			var l=0;
			if (t2>=0) {
				l=1;
			}
			if (t2>=1000*60) {
				l=2;
			}
			if (t2>=1000*60*60) {
				l=3;
			}
			return timeFormat1(t1,l)+' / '+timeFormat1(t2,l);
		};
		var setFrameF=function(iFrame) {
			if (iFrame<0) {
				iFrame=0;
			}
			if (iFrame>=totalFrame-1) {
				iFrame=totalFrame-1;
			}
			if (currentFrameF===iFrame) {
				return ;
			}
			currentFrameF=iFrame;
			txtTime['text'](timeFormat2(currentFrameF*timeStep,(totalFrame-1)*timeStep));
			renderFrame(currentFrameF);
		};
		var render=function() {
			if (!playing) {
				return ;
			}
			var now=getTime();
			var d=(now-lastTime)*speed/timeStep;
			setFrameF(currentFrameF+d);
			lastTime=now;
		};
		var animate=function() {
			render();
			window['requestAnimationFrame'](animate);
		};
		window['requestAnimationFrame'](animate);
		var onplay=function() {
			if (playing) {
				playing=false;
			}
			else {
				playing=true;
				lastTime=getTime();
			}
			updateIcons();
		};
		var initData=function() {
			timeStep=data['frame']['step']*1000;
		};
		var updateSpeedText=function() {
			spanSpeedPower['text'](unshiftChars(speedPower,2,' '));
		};
		var updateIcons=function() {
			iconPlay['classed']({'fa-play':!playing,'fa-pause':playing});
			iconSpeedDirection['classed']({'fa-hand-o-left':speed<0,'fa-hand-o-right':speed>0});
		};
		var onStepLeft=function() {
			if (playing) {
				return ;
			}
			setFrameF(Math['ceil'](currentFrameF)-1);
		};
		var onStepRight=function() {
			if (playing) {
				return ;
			}
			setFrameF(Math['floor'](currentFrameF)+1);
		};
		var onPlayForward=function() {
			if (speed<0) {
				speed=-speed;
				updateIcons();
			}
		};
		var onPlayBackward=function() {
			if (speed>0) {
				speed=-speed;
				updateIcons();
			}
		};
		var onSpeedUp=function() {
			if (speedPower<9) {
				speed*=2;
				speedPower+=1;
				updateSpeedText();
			}
		};
		var onSpeedDown=function() {
			if (speedPower>-9) {
				speed/=2;
				speedPower-=1;
				updateSpeedText();
			}
		};
		var drawControl=function() {
			container=createDraggable();
			container['classed']('control',true);
			var line2=container['append']('div');
			var line3=container['append']('div');
			spanSpeedPower=line2['append']('span')['classed']('speed',true)['text'](2)['append']('sup');
			iconSpeedDirection=line2['append']('i')['classed']('fa',true);
			txtTime=line2['append']('span')['classed']('time',true);
			btnLeftFrame=line3['append']('button')['classed']('unmovable',true);
			btnPlay=line3['append']('button')['classed']('unmovable',true);
			btnRightFrame=line3['append']('button')['classed']('unmovable',true);
			btnBackward=line3['append']('button')['classed']('unmovable',true);
			btnForward=line3['append']('button')['classed']('unmovable',true);
			btnSpeedDown=line3['append']('button')['classed']('unmovable',true);
			btnSpeedUp=line3['append']('button')['classed']('unmovable',true);
			btnLeftFrame['append']('i')['classed']('fa',true)['classed']('fa-step-backward',true);
			btnRightFrame['append']('i')['classed']('fa',true)['classed']('fa-step-forward',true);
			btnForward['append']('i')['classed']('fa',true)['classed']('fa-forward',true);
			btnBackward['append']('i')['classed']('fa',true)['classed']('fa-backward',true);
			btnSpeedUp['append']('i')['classed']('fa',true)['classed']('fa-angle-double-up',true);
			btnSpeedDown['append']('i')['classed']('fa',true)['classed']('fa-angle-double-down',true);
			iconPlay=btnPlay['append']('i')['classed']('fa',true);
			btnLeftFrame['on']('click',onStepLeft);
			btnRightFrame['on']('click',onStepRight);
			btnForward['on']('click',onPlayForward);
			btnBackward['on']('click',onPlayBackward);
			btnSpeedUp['on']('click',onSpeedUp);
			btnSpeedDown['on']('click',onSpeedDown);
			btnPlay['on']('click',onplay);
		};
		return function() {
			initData();
			drawControl();
			updateSpeedText();
			updateIcons();
			setFrameF(0);
		};
	})();
	var Ruler=(function() {
		var tickPosition=[0,1,2,5,10];
		var n=tickPosition['length'];
		var tickSize=tickPosition['map'](function(x,i,a) {
			if (i+1<n) {
				return a[i+1]-a[i];
			}
			else {
				return 0;
			}
		});
		var playgroundWidth,playgroundHeight;
		var container;
		var divSpaces=[];
		var init=function() {
			playgroundWidth=data['playground']['width'];
			playgroundHeight=data['playground']['height'];
			container=createDraggable()['classed']('ruler',true);
			var spanNumber;
			var i;
			for (i=0;i<n;i++) {
				divSpace=container['append']('div')['classed']('space',true);
				spanNumber=divSpace['append']('span')['classed']('number',true);
				spanNumber['text'](tickPosition[i]);
				divSpaces['push'](divSpace);
			}
		};
		var resize=function(windowWidth,windowHeight) {
			if (container===undefined) {
				return ;
			}
			var i;
			var sW=windowWidth/playgroundWidth;
			var sH=windowHeight/playgroundHeight;
			for (i=0;i<n;i++) {
				divSpaces[i]['style']('width',sW*tickSize[i]+'px');
			}
		};
		return {//Ruler
			init:init,
			resize:resize
		};
	})();
	var onWindowResize=(function() {
		var windowWidth,windowHeight;
		return function(forced) {
			var w=window['innerWidth'];
			var h=window['innerHeight'];
			if (forced!==true&&w===windowWidth&&h===windowHeight) {
				return ;
			}
			windowWidth=w;
			windowHeight=h;
			rendering.resize(w,h);
			Ruler.resize(w,h);
		};
	})();
	window['onresize']=onWindowResize;
	var onDomReady=function() {
		document['title']='Monitor';
		loadingScreen.init();
	};
	return onDomReady;
})()();

