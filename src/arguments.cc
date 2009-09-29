//-*- mode: c++; tab-width: 4; indent-tabs-mode: t; c-file-style: "stroustrup"; -*-
// This file is part of wkhtmltopdf.
//
// wkhtmltopdf is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// wkhtmltopdf is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with wkhtmltopdf.  If not, see <http://www.gnu.org/licenses/>.

#include <arghandlers.inc>

/*!
  The next arguments we add will belong to this section
  /param s The name of the section
  /param desc A description of the section
*/
void CommandLineParserPrivate::section(QString s, QString desc) {
	currentSection = s;
	sectionDesc[s] = desc;
	sections.push_back(s);
}

/*!
  Indicate wether the next arguments we add require a patched qt to work
  /param h Do we require a patch
*/
void CommandLineParserPrivate::qthack(bool h) {
	currentHack = h;
}

/*!
  Indicate wether the next arguments we add are "extended" and should not 
  be shown in a simple --help
  \param e Are the arguments extended
*/
void CommandLineParserPrivate::extended(bool e) {
	currentExtended = e;
}

/*!
  Add an argument to the list of arguments
  \param l The long "--" name of the argument
  \param s The short '-' name of the argument or 0 if unspecified
  \param d Descripton of the argument
  \param h The handler for the argument
  \param display Is the argument hidden
*/
void CommandLineParserPrivate::addarg(QString l, char s, QString d, ArgHandler * h, bool display) {
	h->desc = d;
	h->longName = l;
	h->shortSwitch = s;
	h->display = display;
	h->qthack = currentHack;
	h->extended = currentExtended;
	longToHandler[l] = h;
	if(s) shortToHandler[s] = h;
	sectionArgumentHandles[currentSection].push_back(h);
}

/*!
  Construct the commandline parser adding all the arguments
  \param s The settings to store values in
*/
CommandLineParserPrivate::CommandLineParserPrivate(Settings & s):
	settings(s)
{
	section("General Options");
	extended(false);
	addarg("help",'h',"Display help",new Caller<HelpFunc<false> >());
	addarg("quiet",'q',"Be less verbose",new ConstSetter<bool>(s.quiet,true,false));
	addarg("version",'V',"Output version information an exit", new Caller<VersionFunc>());
	addarg("extended-help",0,"Display more extensive help, detailing less common command switches", new Caller<HelpFunc<true> >());
	addarg("collate", 0, "Collate when printing multiple copies", new ConstSetter<bool>(s.collate,true,false));
	addarg("copies", 0, "Number of copies to print into the pdf file", new IntSetter(s.copies, "number", 1));
	addarg("orientation",'O',"Set orientation to Landscape or Portrait", new OrientationSetter(s.orientation, "orientation"));
	addarg("page-size",'s',"Set pape size to: A4, Letter, ect.", new PageSizeSetter(s.pageSize, "size", QPrinter::A4));
	addarg("proxy",'p',"Use a proxy", new ProxySetter(s.proxy, "proxy"));
	addarg("username",0,"HTTP Authentication username", new QStrSetter(s.username, "username",""));
	addarg("password",0,"HTTP Authentication password", new QStrSetter(s.password, "password",""));

	qthack(true);
	addarg("book",'b',"Set the options one would usualy set when printing a book", new Caller<BookFunc>());
	addarg("cover",0,"Use html document as cover. It will be inserted before the toc with no headers and footers",new QStrSetter(s.cover,"url",""));
	addarg("default-header",'H',"Add a default header, with the name of the page to the left, and the page number to the right, this is short for: --header-left='[webpage]' --header-right='[page]/[toPage]' --top 2cm --header-line", new Caller<DefaultHeaderFunc>());
	addarg("toc",'t',"Insert a table of content in the beginning of the document", new ConstSetter<bool>(s.printToc,true,false));
	qthack(false);
	
	extended(true);
	addarg("manpage", 0, "Output program man page", new Caller<ManPageFunc>());
	addarg("htmldoc", 0, "Output program html help", new Caller<ReadmeFunc<true> >());
	addarg("readme", 0, "Output program readme", new Caller<ReadmeFunc<false> >());
	addarg("dpi",'d',"Change the dpi explicitly", new IntSetter(s.dpi,"dpi",-1));
	addarg("disable-javascript",'n',"Do not allow webpages to run javascript", new ConstSetter<bool>(s.enableJavascript,false,true));
	addarg("grayscale",'g',"PDF will be generated in grayscale", new ConstSetter<QPrinter::ColorMode>(s.colorMode,QPrinter::GrayScale,QPrinter::Color));
	addarg("lowquality",'l',"Generates lower quality pdf/ps. Useful to shrink the result document space", new ConstSetter<QPrinter::PrinterMode>(s.resolution,QPrinter::ScreenResolution,QPrinter::HighResolution));
	addarg("margin-bottom",'B',"Set the page bottom margin (default 10mm)", new UnitRealSetter(s.margin.bottom,"unitread",QPair<qreal,QPrinter::Unit>(10,QPrinter::Millimeter)));
	addarg("margin-left",'L',"Set the page left margin (default 10mm)", new UnitRealSetter(s.margin.left,"unitread",QPair<qreal,QPrinter::Unit>(10,QPrinter::Millimeter)));
	addarg("margin-right",'R',"Set the page right margin (default 10mm)", new UnitRealSetter(s.margin.right,"unitread",QPair<qreal,QPrinter::Unit>(10,QPrinter::Millimeter)));
	addarg("margin-top",'T',"Set the page top margin (default 10mm)", new UnitRealSetter(s.margin.top,"unitread",QPair<qreal,QPrinter::Unit>(10,QPrinter::Millimeter)));
	addarg("redirect-delay",0,"Wait some miliseconds for js-redirects", new IntSetter(s.jsredirectwait,"msec",200));
	addarg("enable-plugins",0,"Enable installed plugins (such as flash", new ConstSetter<bool>(s.enablePlugins,true,false));
	addarg("zoom",0,"Use this zoom factor", new FloatSetter(s.zoomFactor,"float",1.0));

	qthack(true);
	addarg("disable-internal-links",0,"Do no make local links", new ConstSetter<bool>(s.useLocalLinks,false,true));
	addarg("disable-external-links",0,"Do no make links to remote webpages", new ConstSetter<bool>(s.useExternalLinks,false,true));
	addarg("print-media-type",0,"Use print media-type instead of screen", new ConstSetter<bool>(s.printMediaType,true,false));
	addarg("page-offset",0,"Set the starting page number", new IntSetter(s.pageOffset,"offset",1));
	addarg("disable-smart-shrinking", 0, "Disable the intelligent shrinking strategy used by webkit that makes the pixel/dpi ratio none constant",new ConstSetter<bool>(s.enableIntelligentShrinking, false, true));
#ifdef Q_WS_X11
	addarg("use-xserver",0,"Use the X server (some plugins and other stuff might not work without X11)", new ConstSetter<bool>(s.useGraphics,true,false));
#endif

#if QT_VERSION >= 0x040600
	qthack(false);
#endif
	addarg("encoding",0,"Set the default text encoding, for input", new QStrSetter(s.defaultEncoding,"encoding",""));
	qthack(false);
	
#if QT_VERSION >= 0x040500 //Not printing the background was added in QT4.5
	addarg("no-background",0,"Do not print background", new ConstSetter<bool>(s.background,false,true));
	addarg("user-style-sheet",0,"Specify a user style sheet, to load with every page", new QStrSetter(s.userStyleSheet,"url",""));
#endif
	
	extended(false);
	section("Headers And Footer Options");
	qthack(true);
	extended(true);
	addarg("footer-center",0,"Centered footer text", new QStrSetter(s.footer.center,"text",""));
	addarg("footer-font-name",0,"Set footer font name", new QStrSetter(s.footer.fontName,"name","Arial"));;
	addarg("footer-font-size",0,"Set footer font size", new IntSetter(s.footer.fontSize,"size",11));
	addarg("footer-left",0,"Left aligned footer text", new QStrSetter(s.footer.left,"text",""));
	addarg("footer-line",0,"Display line above the footer", new ConstSetter<bool>(s.footer.line,true,false));
	addarg("footer-right",0,"Right aligned footer text", new QStrSetter(s.footer.right,"text",""));
	addarg("header-center",0,"Centered header text", new QStrSetter(s.header.center,"text",""));
	addarg("header-font-name",0,"Set header font name", new QStrSetter(s.header.fontName,"name","Arial"));
	addarg("header-font-size",0,"Set header font size", new IntSetter(s.header.fontSize,"size",11));
	addarg("header-left",0,"Left aligned header text", new QStrSetter(s.header.left,"text",""));
	addarg("header-line",0,"Display line below the header", new ConstSetter<bool>(s.header.line,true,false));
	addarg("header-right",0,"Right aligned header text", new QStrSetter(s.header.right,"text",""));
	extended(false);
	qthack(false);
	
	section("Table Of Content Options");
	extended(true);
	qthack(true);
	addarg("toc-font-name",0,"Set the font used for the toc", new QStrSetter(s.toc.fontName,"name","Arial"));
	addarg("toc-no-dots",0,"Do not use dots, in the toc", new ConstSetter<bool>(s.toc.useDots,false,true));
	addarg("toc-depth",0,"Set the depth of the toc", new IntSetter(s.toc.depth,"level",3));
	addarg("toc-header-text",0,"The header text of the toc", new QStrSetter(s.toc.captionText,"text","Table Of Contents"));
	addarg("toc-header-fs",0,"The font size of the toc header", new IntSetter(s.toc.captionFontSize,"size",15));
	addarg("toc-disable-links",0,"Do not link from toc to sections", new ConstSetter<bool>(s.toc.forwardLinks,false, true));
	addarg("toc-disable-back-links",0,"Do not link from section header to toc", new ConstSetter<bool>(s.toc.backLinks,false,true));
	for (uint i=0; i < Settings::TOCSettings::levels; ++i) {
		addarg(QString("toc-l")+QString::number(i+1)+"-font-size",0,QString("Set the font size on level ")+QString::number(i+1)+" of the toc",new IntSetter(s.toc.fontSize[i],"size",12-2*i), i < 3);
		addarg(QString("toc-l")+QString::number(i+1)+"-indentation",0,QString("Set indentation on level ")+QString::number(i+1)+" of the toc",new IntSetter(s.toc.indentation[i],"num",i*20), i < 3);
	}
	qthack(true);
	extended(false);

	section("Outline Options");
	extended(true);
	qthack(true);
	addarg("outline",0,"Put an outline into the pdf", new ConstSetter<bool>(s.outline,true,false));
	addarg("outline-depth",0,"Set the depth of the outline", new IntSetter(s.outlineDepth,"level",4));
	qthack(true);
	extended(false);
}
