---
author: admin
comments: true
date: 2008-07-18 18:16:01+00:00
layout: page
slug: home
title: Home
wordpress_id: 2
---

Welcome to **OCILIB - C and C++ Driver for Oracle** website !

OCILIB is an **open source** and **cross platform** Oracle **Driver** that delivers efficient access to **Oracle databases**.

The OCILIB library  :

  * offers a rich, full featured and easy to use API
  * runs on all Oracle platforms
  * is written in pure ISO C99 code with native ISO C Unicode support
  * provides also a C++ API written in standard C++03
  * Enables high productivity
  * encapsulates OCI (Oracle Call Interface)
  * is the most complete available OCI wrapper

**OCILIB** is developed by [Vincent Rogier]({{site.baseurl}}/about/). Get the latest package, install and enjoy it !
  
## Latest version

The latest version is [version 4.1.0 (2015-05-05)]({{site.projecturl}}/releases/)
 
 <p id="DownloadCount"></p>
 
## Recent Posts

{% for post in site.posts limit:5 site.recent_posts %}
  *  [{% if site.titlecase %}{{ post.title | titlecase }}{% else %}{{ post.title }}{% endif %}]({{ site.baseurl }}{{ post.url }})
{% endfor %}
 
## Testimonies
  
>"OCILIB code is truly a really nice piece of code"
><br/>  
>Pete Finnigan, CEO and Founder of PeteFinnigan.com Limited
>Specialists in database security.

>"About Ocilib, i am working in Telecom Italia S.p.A. and we are using your Ocilib library compiled on HP Itanium System in our Interconnection System in Brazil. In a process to extract daily traffic data from partitioned oracle tables, about 30.000.000 records for each day. This process wrote to use dynamic sql 4 Method had a big performance problem and i have sponsored your ocilib and we have improved +60% performance. I am using your ocilib also in a windows desktop application on XP system, with good results. Thanks Vincent."
><br/>  
>Giovanni Palleschi
>Software Engeener at Telecom Italia S.p.A
      
>"I have been asked to replay 90 minutes of captured SQL code back into a database. The oracle database is one of the largest in the southern hemisphere apparently [....] 90 minutes of captured data is about 2 terabytes of information [...] The replay is all happening on a single client Solaris 64 bit machine [...] From a parent controlling process I spawn 1300 client processes that each connect to the database[...]"
>all calls seems to happen very, very quickly and that, in my opinion, a significant reason the project is succeeding is due to your library and your commitment to quality code.
>Thank you! Well Done! Bravo!
>The system I have written has been designed to really hammer the database and your code stands up very well.
><br/>   
>James D'Arcy
>CommonwealthBank

<script>
 
    var total = 56242; // Value as 2015-07-15 from source forge. Need to add rest call to SF.Net

    function getHTTPObject()
    {
        if (typeof XMLHttpRequest != 'undefined') 
        {
            return new XMLHttpRequest();
        }
        try 
        { 
            return new ActiveXObject("Msxml2.XMLHTTP"); 
        } catch (e)
        { 
            try 
            { 
               return new ActiveXObject("Microsoft.XMLHTTP"); 
            } 
            catch (e)
            {
            } 
        }
        return false;
    }
    
	(function() {
        
		var GetJson = function(url, successHandler, errorHandler) 
        {			
			var xhr = getHTTPObject();
			xhr.open('get', url, true);
			xhr.onload = function() {
				var status = xhr.status;
				if (status == 200) {
					successHandler && successHandler(JSON.parse(xhr.responseText));
				} else {
					errorHandler && errorHandler(status);
				}
			};
			xhr.send();
		};

		GetJson('https://api.github.com/repos/vrogier/ocilib/releases/latest', function(data)
		{			
			for (i in data.assets)
			{
			  total = total + data.assets[i].download_count;
			}				
		});
        
       <!-- 
        GetJson('http://sourceforge.net/projects/orclib/files/stats/json?start_date=2007-1-1&end_date=' + GetDate(), function(data)
		{		
			var json = BufferToJson(data);

			total = total + json.summaries.time.downloads;				
		}); -->

        document.getElementById('DownloadCount').innerHTML =  '<br/>Total of download since first release : ' + '<b>' + total + '</b>';

	}());
</script>
