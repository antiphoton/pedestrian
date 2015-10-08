(function() {
	var data;
	d3['json'](
		'./out.trajectory.json',
		function(a) {
			if (a) {
				data=a;
				checkBoth();
			}
		}
	);
	var svg;
	var people=[];
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
			var maxPeople=data['playground']['maxPeople'];
			var i;
			var person;
			for (i=0;i<maxPeople;i++) {
				person=svg['append']('circle');
				person['attr']('r',0.2);
				person['attr']('display','none');
				people['push'](person);
			}
			switchFrame(80);
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
			var pos=data['position'][iFrame];
			var n=Math['max'](people['length'],pos['length']);
			var i;
			for (i=0;i<n;i++) {
				if (pos[i]!==undefined) {
					people[i]['attr']('display','inline');
					people[i]['attr']('transform','translate('+pos[i][0]+','+pos[i][1]+')');
				}
				else {
					people[i]['attr']('display','none');
				}
			}
		};
	})();
	var createControll=(function() {
		var container;
		var btnLeft,btnRight;
		var currentFrame=0;
		var txtIndex;
		var setFrame=function(iFrame) {
			if (!(iFrame>=0&&iFrame<data['position']['length'])) {
				return ;
			}
			if (iFrame===currentFrame) {
				return ;
			}
			else {
				currentFrame=iFrame
			}
			txtIndex['text'](iFrame);
			switchFrame(iFrame);
		};
		return function() {
			container=d3['select']('body')['append']('span');
			container['classed']('control',true);
			btnLeft=container['append']('button');
			txtIndex=container['append']('span');
			btnRight=container['append']('button');
			btnLeft['text']('<');
			btnRight['text']('>');
			txtIndex['text'](currentFrame);
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

