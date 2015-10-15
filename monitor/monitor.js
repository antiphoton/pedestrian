(function() {
	var getTime=function() {
		return new Date()['getTime']();
	};
	var loadingScreen=(function() {
		var table,tbody;
		var aTr=[];
		var aCaption=[],aTask=[],aFinish=[],aError=[];
		var aStatusChanged=[];
		var total=0;
		var init=function() {
			table=d3['select']('body')['append']('table')['classed']('loading',true);
			tbody=table['append']('tbody');
			var tr;
			var tdCaption,tdStatus;
			var i;
			for (i=0;i<total;i++) {
				tr=tbody['append']('tr');
				aTr['push'](tr);
				tdCaption=tr['append']('td')['classed']('caption',true);
				tdStatus=tr['append']('td')['classed']('status',true);
				tdCaption['text'](aCaption[i]);
				tdStatus['append']('span');
				tdStatus['append']('i')['classed']('fa',true);
			}
			animate();
		};
		var register=function(caption,task) {
			if (task===undefined) {
				task=0;
			}
			var id=total;
			total++;
			aCaption['push'](caption);
			aTask['push'](task);
			aFinish['push'](0);
			aStatusChanged['push'](true);
			return id;
		};
		var appendTask=function(id,task) {
			aTask[id]+=task;
		};
		var finishTask=function(id,finish) {
			if (aFinish[id]===0) {
				aStatusChanged[id]=true;
			}
			aFinish[id]+=finish;
			if (aFinish[id]===aTask[id]) {
				aStatusChanged[id]=true;
			}
			animate();
		};
		var errorTask=function(id,error) {
			aError[id]+=error;
			animate();
		};
		var lastAnimateTime;
		var animate=function() {
			var i;
			var anyStatusChanged=false;
			var everythingFinished;
			var tdStatus,span,fa;
			var now;
			for (i=0;i<total;i++) {
				if (aStatusChanged[i]===true) {
					anyStatusChanged=true;
				}
			}
			now=getTime();
			if (anyStatusChanged===false&&now-lastAnimateTime<50) {
				return ;
			}
			lastAnimateTime=now;
			for (i=0;i<total;i++) {
				tdStatus=aTr[i]['select']('td.status');
				fa=tdStatus['select']('i');
				span=tdStatus['select']('span');
				if (aStatusChanged[i]) {
					if (aTask[i]===0) {
						fa['classed']('invisible',true);
						span['classed']('invisible',true);
					}
					else if (aTask[i]===aFinish[i]) {
						fa['classed']({'invisible fa-spinner fa-pulse':false,'fa-check':true});
						span['classed']('invisible',true);
					}
					else if (aTask[i]===1) {
						fa['classed']({'invisible fa-check':false,'fa-spinner fa-pulse':true});
						span['classed']('invisible',true);
					}
					else {
						fa['classed']('invisible',true);
						span['classed']('invisible',false);
					}
				}
				else {
					span['text']((aFinish[i]/aTask[i]*100)+'%');
				}
				aStatusChanged[i]=false;
			}
			if (anyStatusChanged) {
				everythingFinished=true
				for (i=0;i<total;i++) {
					if (aTask[i]===0||aFinish[i]<aTask[i]) {
						everythingFinished=false;
						break;
					}
				}
				if (everythingFinished) {
					onAllFinished();
				}
			}
		};
		var onAllFinished=function() {
			table['classed']('invisible',true);
			rendering.init();
			initControl();
			Ruler.init();
			onWindowResize();
			renderFrame(0);
		};
		return {//loadingScreen
			init:init,
			register:register,
			appendTask:appendTask,
			finishTask:finishTask,
		};
	})();
	var data;
	var loadJsonData=(function() {
		var jobId=loadingScreen.register("Download",1);
		return function() {
			d3['json'](
				'./out.trajectory.json',
				function(a) {
					if (a) {
						loadingScreen.finishTask(jobId,1);
						data=a;
						maxPeople=data['playground']['maxPeople'];
						totalFrame=data['frame']['total'];
						parseBase64();
					}
					else {
						loadingScreen.errorTask(jobId,1);
					}
				}
			);
		};
	})();
	var parseBase64=(function() {
		var jobId=loadingScreen.register("Decode");
		var parseByte=function(s) {
			return base64js['toByteArray'](s);
		};
		var parseFloat64=function(s) {
			var a=base64js['toByteArray'](s);
			return new Float64Array(a['buffer']);
		}
		return function() {
			loadingScreen.appendTask(jobId,totalFrame);
			data['frames']['forEach'](function (frame) {
				frame['exist']=parseByte(frame['exist']);
				frame['position']=parseFloat64(frame['position']);
				frame['velocity']=parseFloat64(frame['velocity']);
				frame['acceleration']=parseFloat64(frame['acceleration']);
				frame['destGate']=parseByte(frame['destGate']);
				loadingScreen.finishTask(jobId,1);
			});
		}
	})();
	var maxPeople,totalFrame;
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
			currentFrameF=iFrame;
			if (currentFrameF<0) {
				currentFrameF=0;
			}
			if (currentFrameF>=totalFrame-1) {
				currentFrameF=totalFrame-1;
			}
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
			setFrameF(currentFrameF-1);
		};
		var onStepRight=function() {
			if (playing) {
				return ;
			}
			setFrameF(currentFrameF+1);
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
		loadJsonData();
	};
	return onDomReady;
})()();

