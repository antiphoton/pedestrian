(function() {
	var data;
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
	d3['json'](
		'./out.trajectory.json',
		function(a) {
			if (a) {
				data=a;
				maxPeople=data['playground']['maxPeople'];
				totalFrame=data['frame']['total'];
				parseBase64();
				checkBoth();
			}
		}
	);
	var maxPeople,totalFrame;
	var peopleColor=['0000ff','ff0000'];
	var svgRendering=(function() {
		var svg;
		var container;
		var playgroundWidth,playgroundHeight;
		var windowWidth,windowHeight;
		var people=[];
		var onResize=function() {
			var w=window['innerWidth'];
			var h=window['innerHeight'];
			if (w===windowWidth&&h===windowHeight) {
				return ;
			}
			windowWidth=w;
			windowHeight=h;
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
			onResize();
			window['onresize']=onResize;
			var i;
			var person;
			for (i=0;i<maxPeople;i++) {
				person=svg['append']('circle');
				person['attr']('r',0.2);
				people['push'](person);
			}
			renderFrame(0);
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
			render:render
		};
	})();
	var threeRendering=(function() {
		var canvas;
		var scene;
		var camera;
		var renderer;
		var playgroundWidth,playgroundHeight;
		var windowWidth,windowHeight;
		var people=[];
		var peopleGeometry=new THREE['CircleGeometry'](0.2);
		var peopleMaterial=peopleColor['map'](function(x) {
			return new THREE['LineBasicMaterial']({'color':parseInt(x,16)});
		});
		var onResize=function() {
			var w=window['innerWidth'];
			var h=window['innerHeight'];
			if (w===windowWidth&&h===windowHeight) {
				return ;
			}
			windowWidth=w;
			windowHeight=h;
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
			onResize();
			window['onresize']=onResize;
			var background=new THREE['Mesh'](new THREE['BoxGeometry'](playgroundWidth,playgroundHeight,0.1),new THREE['LineBasicMaterial']({'color':0xefefef}));
			background['position']['set'](playgroundWidth/2,playgroundHeight/2,-1);
			scene['add'](background);
			var i;
			var person;
			for (i=0;i<maxPeople;i++) {
				person=new THREE['Mesh'](peopleGeometry,peopleMaterial[0]);
				scene['add'](person);
				people['push'](person);
			}
			renderFrame(0);
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
		var btnLeft,btnRight;
		var btnForward,btnBackward;
		var btnPlay;
		var txtIndex;
		var iconPlay;
		var currentFrameF=0;
		var lastTime;
		var playing=false;
		var timeStep;
		var speed=1;
		var getTime=function() {
			return new Date()['getTime']();
		};
		var setFrameF=function(iFrame) {
			currentFrameF=iFrame;
			if (currentFrameF<0) {
				currentFrameF=0;
			}
			if (currentFrameF>=totalFrame-1) {
				currentFrameF=totalFrame-1;
			}
			txtIndex['text'](Math['floor'](currentFrameF)+'/'+totalFrame);
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
				iconPlay['classed']('fa-pause',false)['classed']('fa-play',true);
			}
			else {
				playing=true;
				lastTime=getTime();
				iconPlay['classed']('fa-play',false)['classed']('fa-pause',true);
			}
		};
		var onpause=function() {
			if (!playing) {
				return ;
			}
		};
		var initData=function() {
			timeStep=data['frame']['step']*1000;
		};
		var onStepLeft=function() {
			if (playing) {
				return ;
			}
			setFrameF(currentFrameF-1);
		};
		var onStepRight=function() {
			if (playing) {
				return ;
			}
			setFrameF(currentFrameF+1);
		};
		var onFastLeft=function() {
			if (speed>0) {
				speed=-1;
			}
			else {
				speed*=2;
			}
		};
		var onFastRight=function() {
			if (speed<0) {
				speed=1;
			}
			else {
				speed*=2;
			}
		};
		var drawControl=function() {
			container=createDraggable();
			var line2=container['append']('div');
			var line3=container['append']('div');
			txtIndex=line2['append']('span');
			btnLeft=line3['append']('button')['classed']('unmovable',true);
			btnPlay=line3['append']('button')['classed']('unmovable',true);
			btnRight=line3['append']('button')['classed']('unmovable',true);
			btnBackward=line3['append']('button')['classed']('unmovable',true);
			btnForward=line3['append']('button')['classed']('unmovable',true);
			btnLeft['append']('i')['classed']('fa',true)['classed']('fa-step-backward',true);
			btnRight['append']('i')['classed']('fa',true)['classed']('fa-step-forward',true);
			btnForward['append']('i')['classed']('fa',true)['classed']('fa-forward',true);
			btnBackward['append']('i')['classed']('fa',true)['classed']('fa-backward',true);
			iconPlay=btnPlay['append']('i')['classed']('fa',true)['classed']('fa-play',true);
			txtIndex['text'](currentFrameF+'/'+totalFrame);
			btnLeft['on']('click',onStepLeft);
			btnRight['on']('click',onStepRight);
			btnBackward['on']('click',onFastLeft);
			btnForward['on']('click',onFastRight);
			btnPlay['on']('click',onplay);
		};
		return function() {
			initData();
			drawControl();
		};
	})();
	var checkBoth=(function() {
		var total=2;
		var finished=0;
		var main=function() {
			rendering.init();
			initControl();
		};
		return function() {
			finished++;
			if (finished===total) {
				main();
			}
		}
	})();
	var onDomReady=function() {
		document['title']='Monitor';
		checkBoth();
	};
	return onDomReady;
})()();
