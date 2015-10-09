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
			});
		}
	})();
	d3['json'](
		'./out.trajectory.json',
		function(a) {
			if (a) {
				data=a;
				maxPeople=data['playground']['maxPeople'];
				totalFrame=data['frames']['length'];
				parseBase64();
				checkBoth();
			}
		}
	);
	var svg;
	var people=[];
	var maxPeople,totalFrame;
	var createSvg=(function() {
		var container;
		var playgroundWidth,playgroundHeight;
		var windowWidth,windowHeight;
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
		return function() {
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
				person['attr']('display','none');
				people['push'](person);
			}
			switchFrame(0);
		}
	})();
	var switchFrame=(function() {
		var lastFrame;
		return function(iFrame) {
			if (iFrame===lastFrame) {
				return ;
			}
			else {
				lastFrame=iFrame;
			}
			var i;
			var frame=data['frames'][iFrame];
			for (i=0;i<maxPeople;i++) {
				if (frame['exist'][i]) {
					people[i]['attr']('display','inline');
					people[i]['attr']('transform','translate('+frame['position'][i*2+0]+','+frame['position'][i*2+1]+')');
				}
				else {
					people[i]['attr']('display','none');
				}
			}
		};
	})();
	var createDraggable=function() {
		var obj=d3['select']('body')['append']('span');
		var xObj=0,yObj=0;
		obj['classed']('movable',true);
		obj['style']({'position':'fixed','top':xObj,'left':yObj});
		var dragging=false;
		var xLast,yLast;
		var beginMove=function() {
			if (d3.select(d3.event.target).classed('unmovable')) {
				return ;
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
	var createControll=(function() {
		var container;
		var btnLeft,btnRight;
		var txtIndex;
		var currentFrame=0;
		var setFrame=function(iFrame) {
			if (!(iFrame>=0&&iFrame<totalFrame)) {
				return ;
			}
			if (iFrame===currentFrame) {
				return ;
			}
			else {
				currentFrame=iFrame;
			}
			txtIndex['text'](iFrame+'/'+totalFrame);
			switchFrame(iFrame);
		};
		return function() {
			container=createDraggable();
			var line2=container['append']('div');
			var line3=container['append']('div');
			txtIndex=line2['append']('span');
			btnLeft=line3['append']('button')['classed']('unmovable',true);
			btnRight=line3['append']('button')['classed']('unmovable',true);
			btnLeft['text']('<');
			btnRight['text']('>');
			txtIndex['text'](currentFrame+'/'+totalFrame);
			btnLeft['on']('click',function() {
				setFrame(currentFrame-1);
			});
			btnRight['on']('click',function() {
				setFrame(currentFrame+1);
			});
		};
	})();
	var checkBoth=(function() {
		var total=2;
		var finished=0;
		var main=function() {
			createSvg();
			createControll();
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
