---
author: admin
comments: true
date: 2010-02-18 21:23:33+00:00
layout: post
slug: icinga-chose-ocilib-as-oracle-db-layer
title: ICINGA chose OCILIB as Oracle DB layer
wordpress_id: 448
categories:
- News
tags:
- icinga
- monitoring
- network
- OCILIB
- oracle
---

![Icinga, a popular open source monitoring system](http://ocilib.net/public/images/icinga.jpeg)

[Icinga](http://www.icinga.org) is an enterprise grade open source monitoring system which keeps watch over a network and any conceivable network resource, notifies the user of errors and recoveries, and generates performance data for reporting.

The Icinga project was started as a Nagios fork in 2009 and gets really popular by now !

The Icinga core team decided to support Oracle database as backend RDBMS using OCILIB :





  * Icinga Core 1.0.0 was released in December 2009 and added support for Oracle with OCILIB


  * Icinga Core 1.0.1 will be released in March 2010 and brings lots of improvements ([see Icinga blog post here](http://www.icinga.org/2010/02/15/icinga-idoutils-more-improvements-part-i/))




Few weeks ago, i asked Michael Friedrich, member of the Icinga Team Core who implemented Icinga Oracle DB layer with OCILIB, to give me his feelings and feedback about OCILIB and its integration in Icinga.

So, here is his experience directly reported :



<blockquote>First of all, at the very beginning of Icinga IDOUtils, the core dev team decided to use libdbi as database abstraction layer. Because it's easy, install the libdbi as api and compile that code. Afterwards, just install the appropriate driver and everything would be fine.
I have joined Icinga because of getting the former NDOUtils Oracle implementation back into the upstream and improve the code as is. This wouldn't have been possible with Nagios and the ongoing business plans, but that's another topic. It was a bit hard to decide in the first time to drop the liboci wrapper done by David Schmidt and stepping over to fresh and new libdbi.

Oracle and libdbi had a nice start, there was a driver - not yet stable but a least there was code to compile and run. My first tests with the Oracle driver produced core dumps and the database server suffered a lot. So I decided to step deeper into my research of getting a satisfying Oracle driver. On the libdbi mailing lists I have read the proposal just to wrap ocilib into libdbi instead of a full rewrite of the driver. But for what it's worth, the libdbi api does not support all the nice features of ocilib - now I know plenty of them.

I made a development decision then - either wait for an implementation of a stable libdbi driver for Oracle (that never happened til now) or implement that myself (a full new project and full api rewrite) or just use the ocilib as is (this was the right decision).
Old NDOUtils also included code split between the different RDBMS by using defines and other coding stuff. So I was familiar with dividing code into different sections based on the database abstraction layer.

One of my first steps was to read the documentation and also get the idea how to use the ocilib within the current IDOUtils code. Since there are not that many projects offering code examples I was very thankful that the documentation offers so many details and code examples. Getting an initial database connection to work while preparing the code for preparing queries for Oracle was the initial todo.
In the end of August 2009 I finished the code preparations (a new file where the query preparation has been seperated from the original code) and also implemented a basic Postgresql support into Icinga IDOUtils. So work was done just to get all Oracle related stuff into the first version of IDOUtils Oracle.
In the end of October for upcoming Icinga 1.0RC1 I have played around a lot with ocilib. It was about 2 months of coding, testing, fixing, and getting initial Oracle support through ocilib.

Just like the tutorial mentioned, I included the ocilib.h and added connection-/statement-/resultset handlers. Also changed configure to a new flag --enable-oracle and then setting the appropriate compile flags. This was the first time I needed to ask for help from Vincent because of the difference on compiling ocilib statically into IDOUtils or loading it at runtime. After resolving this issue and adding a runtime path to the original init scripts to get ocilib loaded at runtime, another problem was the HAVE_CONFIG directive where I needed to temporarly hack the define in order to include ocilib.h correctly. It was a hack because original Icinga config.h differs from the one ocilib wants to get. But this was resolved too and the initial include of ocilib was just fine.
Next bit attempt was to get all ~50 MERGE queries back into the code and applying this patch set into IDOUtils - and naturally how to use ocilib to get those queries run against Oracle database.

Just in time before upcoming OSMC 2009 in the end of October (and also Icinga 1.0RC) work was done. It was really nice working the ocilib documentation at this stage, because I found out how to handle autocommit, charset conventions and clobs for varchar2 bigger than 4092 characters. Overall it was a lot of work but it would never have succeeded that far without such a good documentation and also the help and code reviews from Vincent.
At this time I did not lean back - there were several issues to fix and I also needed to add ocilib lib and include directories to configure - if they are setup elsewhere, code would not compile.

After a short break there was more work to do. The old attempt was to create queries and always send them directly to the RDBMS. Which is not a good choice if you get the change to prepare statements and just bind parameters at execution time. I discussed a lot about that fact with DBAs at my work place. It came up with the staging solution to implement the most often called queries into prepared statements. So did I and it was another performance boost using Oracle. The documentation was again very useful at this stage - I learned how to prepare the statements, how to bind them and how to improve the code as is :)
Furthermore the code was depending on libdbi too because of the basic implementation of Oracle support. libdbi dependancy has been completely dropped and IDOUtils with Oracle only depend on installed ocilib (with Oracle Instandclient and SDK e,g,). Icinga documentation has beed adapted too and there lots of people were excited about those improvements and reported bugs, feature requests and other stuff. And regarding the fact that IDOUtils Oracle is one of my projects at work, I needed to get a fully working version soon.

You might ask yourself what was missing - well by chance any query should be prepared and binded. Also to mention the periodic cleanup of historical tables interfering with the normal insert or update queries. And last but not least the origin NDOUtils Oracle containted a simulated "get the last insert id" functionality. This was one autoincrement sequence and insert triggers setting the appropriate values. Getting the last inserted id was done by a simple select into that sequence - not really a performance boost. Those issues remained on my todo list.

In the end of January I got my hands back on the code and I decided to do a full rewrite of all queries based upon preparing and binding. But first, I've been adding several indexes on often used tables in order to improve select/delete/update statements. The bigger the tables the longer those queries will take.
After getting a small performance boost from that I decided to drop the autocincrement stuff and create a sequence for each table. It was a lot easier setting the primary key (id) to seq_name.currval while doing an insert than the other way around with a trigger and an issued select then.
Also returning the now inserted id was quite a nice approach, but it did not fit for MERGE statements - the tricky part because MERGE normally uses two tables and then doing an insert or update based on the unique constraint. IDOUtils uses that statement just to emulate the MySQL feature "insert on duplicate key update". So to speak, another roundtrip meaning a select on each table's sequence needed to be done. But for what it's worth, it does not cost a lot.
The biggest part was to get all queries into statement handlers and to tell ocilib what to do - first prepare them and at execution time just bind the values to those parameters. At some places I got into datatype issues but anything resolved into a fully working version. Just to mention - for dynamic delete I have been implementing two procedures where table and fields may be binded at runtime. ocilib also supports running procedures and this was really nice and easy to apply.
The worst part was forgetting a small statement to close (one which has not been prepared but used all the time) and getting lots of open cursors. Just in time I asked Vincent about that I found out more about that and then resolved this issue by myself. In the end it was a real pleasure to have one to ask but my learning progress was very fast to resolve most issues by myself though :)

Getting that much help and a real nice documentation from Vincent for free was really great and helped a lot getting IDOUtils Oracle to a stage where it is a really evolved version. Upcoming Icinga 1.0.1 on 3rd of March 2010 will have all mentioned fixes and improvements - and also to mention, if you are interested in Icinga, take a look into the THANKS file ;-)

Michael Friedrich
</blockquote>



Have a nice weekend :)

