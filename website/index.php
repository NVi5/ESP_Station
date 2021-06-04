<!DOCTYPE HTML>
<html>

  <head>
	<meta charset="utf-8"/>
	<title>ESP32 MMPS</title>
	<link rel="stylesheet" href="style.css?<?=filemtime("style.css")?>" type="text/css">
    <script src="https://code.jquery.com/jquery-3.0.0.min.js" integrity="sha256-JmvOoLtYsmqlsWxa7mDSLMwa6dZ9rrIdtrrVYRnDRH0=" crossorigin="anonymous"></script>
	<script src="https://code.highcharts.com/stock/highstock.js"></script>
	<script type="text/javascript" src="script.js?<?=filemtime("script.js")?>"></script>
  </head>

  <body>
	<div id="top">
	  MMPS - ESP32
    </div>

	<div id="center">
	
	  <div id="values">
	    <div class="value" id="temp">
		  TEMPERATURA
	    </div>
	    <div class="value" id="pres">
		  CIŚNIENIE
	    </div>
	    <div class="value" id="humi">
		  WILGOTNOŚĆ
	    </div>
	  </div>
	  
	  <div class="clear:both;"></div>
	  
      <div id="plots">
        <div class="plot" id="chart_temp">
          WYKRES TEMPERATURY
	    </div>
        <div class="plot" id="chart_humi">
          WYKRES WILGOTNOŚCI
		</div>
        <div class="plot" id="chart_pres">
          WYKRES CIŚNIENIA
	    </div>
	  </div>
	</div>

	<div id="bottom">
	  STRONA WYKONANA W RAMACH PROJEKTU NA PRZEDMIOT<br>METODYKI MODELOWANIA I PROJEKTOWANIA SYSTEMÓW
	</div>

  </body>
</html>