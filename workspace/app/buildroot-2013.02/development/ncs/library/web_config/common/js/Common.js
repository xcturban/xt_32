var b_help_button=1;/* if display help page */
var c_language=0;

var	c_str1="请输入ID号！";
var	c_str2="错误的ID号，请重新输入！";
var	c_str3="请输入IP地址！";
var	c_str4="错误的IP地址，请重新输入！";
var	c_str5="请输入子网掩码！";
var	c_str6="错误的子网掩码，请重新输入！";
var	c_str7="请输入网关地址！";
var	c_str8="错误的网关地址，请重新输入！";
var	c_str9="请输入服务器地址！";
var	c_str10="错误的服务器地址，请重新输入！";
var	c_str11="请输入端口号(0-65534)！";
var	c_str12="错误的端口号，请重新输入(1-65534)！";
var	c_str13="请输入音量(0-15)！";
var	c_str14="错误的音量，请重新输入(0-15)！";
var	c_str15="请输入MAC地址！";
var	c_str16="错误的MAC地址，请重新输入！";
function changeCommonLanguage(lan){
  switch(lan){
    case 0:
	c_str1="请输入ID号！";
	c_str2="错误的ID号，请重新输入！";
	c_str3="请输入IP地址！";
	c_str4="错误的IP地址，请重新输入！";
	c_str5="请输入子网掩码！";
	c_str6="错误的子网掩码，请重新输入！";
	c_str7="请输入网关地址！";
	c_str8="错误的网关地址，请重新输入！";
	c_str9="请输入服务器地址！";
	c_str10="错误的服务器地址，请重新输入！";
	c_str11="请输入端口号(0-65534)！";
	c_str12="错误的端口号，请重新输入(1-65534)！";
	c_str13="请输入音量(0-15)！";
	c_str14="错误的音量，请重新输入(0-15)！";
	c_str15="请输入MAC地址！";
	c_str16="错误的MAC地址，请重新输入！";
	break;

    case 1:
	c_str1="Input ID please.";
	c_str2="Illegal ID, try again!";
	c_str3="Input IP please.";
	c_str4="Illegal IP, try again!";
	c_str5="Input NetMask please.";
	c_str6="Illegal NetMask, try again!";
	c_str7="Input NetGate please.";
	c_str8="Illegal NetGate, try again!";
	c_str9="Input server address please.";
	c_str10="Illegal server address, try again!";
	c_str11="Input port please(0-65534)!";
	c_str12="Illegal port, try again(1-65534)!";
	c_str13="Input volume please(0-15)!";
	c_str14="Illegal volume, try again(0-15)!";
	c_str15="Input MAC please.";
	c_str16="Illegal MAC, try again!";
	break;
  }
}

function GetCookie(c_name) { 
	var c_start, c_end; 
	if (document.cookie.length > 0) { 
		c_start = document.cookie.indexOf(c_name + "="); 
		if (c_start != -1) { 
			c_start = c_start + c_name.length + 1; 
			c_end = document.cookie.indexOf(";",c_start); 
			if (c_end == -1) 
				c_end = document.cookie.length; 
				
				return unescape(document.cookie.substring(c_start,c_end)); 
		} 
	} 
	
	return "";
}

function SetCookie(c_name,value,expires,path) { 
	if(expires==0){
		document.cookie=c_name + "=" + escape(value) + ";path="+ path;
	}else{
	 	var date = new Date(); 
		date.setTime(date.getTime()+expires); 
		document.cookie = c_name + "=" + escape(value) + ";expires=" + date.toGMTString()+ ";path="+path;
	}
}

function GetMinWidth() { 
	return Math.ceil((window.screen.width - 160) * 0.55) - 6;
}

function Resize(Obj) { 
	var MinWidth = GetMinWidth(); 
	if (window.document.body.offsetWidth > MinWidth) 
		Obj.autoWidth.style.width = "100%"; 
	else 
		Obj.autoWidth.style.width = MinWidth; 
		
	return true; 
}

function LoadNext(FileName) { 
	if(window.parent != window) 
		window.parent.mainFrame.location.href = FileName; 
		
		return true;
}

function IpVerify(IpAddr) { 
	var c; 
	var n = 0; 
	var ch = ".0123456789"; 
	
	if ((IpAddr.length < 7) || (IpAddr.length > 15)) 
		return false; 
	for (var i = 0; i < IpAddr.length; i++) { 
		c = IpAddr.charAt(i); 
		if (ch.indexOf(c) == -1) { 
			return false; 
		} else { 
			if (c == '.') { 
				if (IpAddr.charAt(i+1) != '.') 
					n++; 
				else 
					return false; 
			} 
		} 
	} 
	if (n != 3) 
		return false; 
	if ((IpAddr.indexOf('.') == 0) || (IpAddr.lastIndexOf('.') == (IpAddr.length - 1))) 
		return false; Item = [0,0,0,0]; 
	var Remain; 
	for (var i = 0; i < 3; i++) { 
		var n = IpAddr.indexOf('.'); 
		Item[i] = IpAddr.substring(0, n); 
		Remain = IpAddr.substring(n + 1); 
		IpAddr = Remain; 
	} 
	Item[3] = Remain; 
	for (var i = 0; i < 4; i++) { 
		if ((Item[i] < 0) || (Item[i] > 255)) return false; 
	} 
	return true; 
}

function IsID(Id) { 
	if (Id.length == 0) { 
		alert(c_str1); 
		return false; 
	} 
	if (!IsNumber(Id,1,65535)){ alert(c_str2); 
		return false; 
	} 	
	return true;
}


function IsIp(Ip) { 
	if (Ip.length == 0) { 
		alert(c_str3); 
		return false; 
	} 
	if (!IpVerify(Ip)){ alert(c_str4); 
		return false; 
	} 
	return true;
}

function IsMask(Mask) { 
	if (Mask.length == 0) { 
		alert(c_str5); 
		return false; 
	} 
	if (!IpVerify(Mask)){ 
		alert(c_str6); 
		return false; 
	} 
	return true;
}

function IsGateway(Gateway) { 
	if (Gateway.length == 0) { 
		alert(c_str7); 
		return false; 
	} 
	if (!IpVerify(Gateway)) { 
		alert(c_str8); 
		return false; 
	} 
	return true;
}

function IsSever(Sever) { 
	if(Sever.length == 0) { 
		alert(c_str9); 
		return false; 
	} 
	if (!IpVerify(Sever)) { 
		alert(c_str10); 
		return false; 
	} 
	return true;
}

function IsNumber(NumberString, Min, Max) { 
	var c; 
	var ch = "0123456789"; 
	for (var i = 0; i < NumberString.length; i++) { 
		c = NumberString.charAt(i); 
		if (ch.indexOf(c) == -1) 
			return false; 
	} 
	return ((parseInt(NumberString) >= Min) && (parseInt(NumberString) <= Max))
}

function IsPort(Port) { 
	if (Port.length == 0) { 
		alert(c_str11); 
		return false; } 
		if (!IsNumber(Port,0,65534)) { 
			alert(c_str12); 
			return false; 
		} 
		return true;
}

function IsVolume(Volume) { 
	if (Volume.length == 0) { 
		alert(c_str13); 
		return false; 
	} 
	if (!IsNumber(Volume,0,15)) { 
		alert(c_str14); 
		return false; 
	} return true;
}
	
function CharCompare(szname,en_limit,cn_limit) { 
	var c; 
	var ch = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_.><,[]{}?/+=|\\'\":~!#$%()` & "; 
	if (szname.length > en_limit) 
		return false; 
	for (var i = 0; i < szname.length; i++) { 
		c = szname.charAt(i); 
		if (ch.indexOf(c) == -1) { 
			if(szname.length > cn_limit) 
			return false; 
		} 
	} 
	return true;
}

function MacVerify(MacAddr) { 
	var c; 
	var n = 0; 
	var ch = "0123456789ABCDEFabcdef"; 
	if (MacAddr.length != 17) 
		return false; 
	for (var i = 0; i < MacAddr.length; i++) { 
		c = MacAddr.charAt(i); 
	if (c == '-') 
		n++; 
	else if (ch.indexOf(c) == -1) 
		return false; } 
	if (n != 5) 
		return false; 
	for (var i = 2; i < 17; i += 3) { 
		if (MacAddr.charAt(i) != '-') 
			return false; 
	} 
	return true; 
}

function IsMac(Mac) { 
	if (Mac.length == 0) { 
		alert(c_str15); 
			return false; 
	} 
	if (!MacVerify(Mac)) { 
		alert(c_str16); 
			return false; 
	} 
	return true; 
}

function SetFocus(Element) { 
	if (Element) { 
		Element.focus(); 
		Element.select(); 
	}
}

function IsOptionalAddr(IP, Port) { 
	if ((IP.value != "") && (!IsIp(IP.value))) { 
		SetFocus(IP); 
		return false; 
	} 
	if ((Port.value != "") && (!IsPort(Port.value))) { 
		SetFocus(Port); 	return false; 
	} 
	if ((IP.value == "") && (Port.value != "")) { 
		alert(c_str3); 
		SetFocus(IP); 
		return false; 
	} 
	if ((IP.value != "") && (Port.value == "")) { 
		alert(c_str11); 
		SetFocus(Port); 
		return false; 
	} 
	return true;
}

function IsRequiredAddr(IP, Port) { 
	if (!IsIp(IP.value)) { 
		SetFocus(IP); 
		return false; 
	} 
	if (!IsPort(Port.value)) { 
		SetFocus(Port); 
		return false; 
	} 
	return true;
}

function num2bool(vnum){
	if(vnum==0){
		return false;
	}else{
		return true;
	}
}
function bool2num(vbool){
	if(vbool==true){
		return 1;
	}else{
		return 0;
	}	
}

function $(id){
	return document.getElementById(id);
}

function setValue(id,val){
	$(id).value=val;
}

function urlEncode(url){
	var str="";
	for(i=0;i<url.length;i++){
		switch(url.charAt(i)){
			case '?':
				str+='1';
				str+='1';
				break;
			case '=':
				str+='1';
				str+='2';
				break;
			case '&':
				str+='1';
				str+='3';
				break;
			case ';':
				str+='1';
				str+='4';
				breaK;
			case '/':
				str+='1';
				str+='5';
				break;
			default:
				str+='0';
				str+=url.charAt(i);
				break;
		}
	}
	return str;
}

function select_option(i_start, i_end, i_offset, str_pre, str_tail){
  var i=0;
  var tmp;
  
  for(i=i_start; i<i_end; i++){
    document.write("<option value="+i+" >");
    tmp=i+i_offset;
    document.write(str_pre+tmp+str_tail);
    document.write("</option>");
  }
}

function setValue(id,val){
	$(id).value=val;
}

function put_radio(name, value){
  obj=document.getElementsByName(name);
  for(i=0; i<obj.length; i++){
    if(obj[i].value==value){
      obj[i].checked=true;
      break;
    }
  }
}


function get_radio(name){
  obj=document.getElementsByName(name);
  for(i=0; i<obj.length; i++){
    if(obj[i].checked==true){
      return obj[i].value;
    }
  }
}

function put_checkbox(name, value){
  var list=value;
  var current=0;
  var obj=document.getElementsByName(name);
  for(i=0; i<obj.length; i++){
    current=list%2;
    list=parseInt(list/2);
    obj[i].checked=(current==1)?true:false;
  }
}


function get_checkbox(name){
  var list=0;
  var current=0;
  var obj=document.getElementsByName(name);
  for(i=obj.length-1; i>-1; i--){
    current=(obj[i].checked==true)?1:0;
    list=list*2+current;
  }
  return list;
}

function ini2json(data){  
    var i;
    var len;
    var line;
    var regex = {  
        section: /^\s*\[\s*([^\]]*)\s*\]\s*$/,  
        param: /^\s*([\w\.\-\_]+)\s*=\s*(.*?)\s*$/,  
        comment: /^\s*;.*$/  
    };  
    var value = {};  
    var lines = data.split(/\r\n|\r|\n/);  
    var section = null; 
    len = lines.length;
    for(i=0; i<len; i++){
      line=lines[i];
      if(regex.comment.test(line)){ 
          return;  
      }else if(regex.param.test(line)){ 
          var match = line.match(regex.param); 
          if(section){ 
              value[section][match[1]] = match[2];  
          }else{  
              value[match[1]] = match[2]; 
          } 
      }else if(regex.section.test(line)){ 
          var match = line.match(regex.section);  
          value[match[1]] = {};  
          section = match[1]; 
      }else if(line.length == 0 && section){  
          section = null;  
      }      
    }
    return value;  
}

function iniHelpButton(){
  if($('help_button')){
    /*b_help_button=1;*/
    if(b_help_button==1){
      $('help_right').className="help_right";
      $('help_button').className="help_button";
    }
    $('help_button').onclick=function(){
      b_help_button=b_help_button==1?0:1;
      if(b_help_button==0){
        $('help_right').className="help_right_0";
        $('help_button').className="help_button_0";
      }else{
        $('help_right').className="help_right";
        $('help_button').className="help_button";
      }
    }
  }
}
