function newMainCol(color,ndx,text) {
    if (color=='') {
        col  = "<TD ALIGN=center HEIGHT=25>";
    } else {
        col  = "<TD ALIGN=center HEIGHT=25 BGCOLOR="+color+">";
    }
    col += "<A HREF=\"javascript:top.createMainMenu("+ndx+");\"\n";
    col += "><B>"+text+"</B></A>";
    col += "</TD>\n";
    return col;
}
function createMainMenu(evid) {
    colorHi = '';
    colorLo = 'gray';
    page  = "<HTML>\n";
    page += "<HEAD>\n";
    page += "<STYLE TYPE='text/css'>\n";
    page += "  A {\n";
    page += "    color: #000000;\n";
    page += "    text-decoration: none;\n";
    page += "    font-family: courier;\n";
    page += "    font-size: 14pt;\n";
    page += "  }\n";
    page += "</STYLE>\n";
    page += "</HEAD>\n";
    page += '<BODY BGCOLOR="gray">\n';
    page += "<TABLE\n";
    page += "    BORDER='0'\n";
    page += "    CELLPADDING='0'\n";
    page += "    CELLSPACING='0'\n";
    page += "    WIDTH='100%'\n";
    page += "    BGCOLOR='#C3C3C3'\n";
    page += "    HSPACE='0'\n";
    page += "    VSPACE='0'\n";
    page += ">\n";
    page += "<TR>\n";
    page += top.newMainCol (
        (evid==1)?colorHi:colorLo,
		'1',
        'NAMES<BR>REFERENCE'
    );
    page += top.newMainCol (
        (evid==2)?colorHi:colorLo,
        '2',
        'FUNCTIONS<BR>REFERENCE'
    );
    page += "</TR>\n";
	page += "</TABLE>\n";
    page += "</BODY>\n";
    page += "</HTML>\n";
    top.ReferenceMenu.document.close();
    top.ReferenceMenu.document.open();
    top.ReferenceMenu.document.write(page);
    top.ReferenceMenu.document.close();
    if (evid==1) {
	    top.Index.location.replace (
            'pnames.html'
        );
    } else if (evid==2) {
	    top.Index.location.replace (
            'pfuncs.html'
        );
    }
}
