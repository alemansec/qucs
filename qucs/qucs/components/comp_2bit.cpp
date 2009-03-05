/***************************************************************************
                             comp_2bit
                            -----------
    begin                : December 2008
    copyright            : (C) 2008 by Mike Brinson
    email                : mbrin72043@yahoo.co.uk
 ***************************************************************************/

/*
 * comp_2bit.cpp - device implementations for comp_2bit module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#include <stdlib.h>

#include "comp_2bit.h"
#include "node.h"
#include "main.h"

comp_2bit::comp_2bit()
{
  Type = isComponent; // Analogue and digital component.
  Description = QObject::tr ("2bit comparator verilog device");

  Props.append (new Property ("TR", "6", false,
    QObject::tr ("transfer function high scaling factor")));
  Props.append (new Property ("Delay", "1 ns", false,
    QObject::tr ("output delay")
    +" ("+QObject::tr ("s")+")"));
 
  createSymbol ();
  tx = x1 + 19;
  ty = y2 + 4;
  Model = "comp_2bit";
  Name  = "Y";
}

Component * comp_2bit::newOne()
{
  comp_2bit * p = new comp_2bit();
  p->Props.getFirst()->Value = Props.getFirst()->Value; 
  p->recreate(0); 
  return p;
}

Element * comp_2bit::info(QString& Name, char * &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("2Bit Comparator");
  BitmapFile = (char *) "comp_2bit";

  if(getNewOne) return new comp_2bit();
  return 0;
}

void comp_2bit::createSymbol()
{
  Lines.append(new Line(-40, -50, 40,-50,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 40, -50, 40, 60,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 40,  60,-40, 60,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-40,  60,-40, -50,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-60,-10,-40,-10,QPen(QPen::darkBlue,2)));  // X0
  Lines.append(new Line(-60, 10,-40, 10,QPen(QPen::darkBlue,2)));  // X1
  Lines.append(new Line(-60, 30,-40, 30,QPen(QPen::darkBlue,2)));  // Y0
  Lines.append(new Line(-60, 50,-40, 50,QPen(QPen::darkBlue,2)));  // Y1
  Lines.append(new Line( 40, 30, 60, 30,QPen(QPen::darkBlue,2)));  // L
  Lines.append(new Line( 40, 10, 60, 10,QPen(QPen::darkBlue,2)));  // G
  Lines.append(new Line( 40,-10, 60,-10,QPen(QPen::darkBlue,2)));  // E

  Texts.append(new Text(-25,-45, "COMP", QPen::darkBlue, 12.0));

  Texts.append(new Text(-25,-20,   "{",  QPen::darkBlue, 16.0));
  Texts.append(new Text(-15,-15,   "X",  QPen::darkBlue, 12.0));
  Texts.append(new Text(-35,-23,   "0",  QPen::darkBlue, 12.0));
  Texts.append(new Text(-35, -3,   "1",  QPen::darkBlue, 12.0));
  Texts.append(new Text(-25, 22,   "{",  QPen::darkBlue, 16.0));
  Texts.append(new Text(-15, 27,   "Y",  QPen::darkBlue, 12.0));
  Texts.append(new Text(-35, 17,   "0",  QPen::darkBlue, 12.0));
  Texts.append(new Text(-35, 37,   "1",  QPen::darkBlue, 12.0));
  Texts.append(new Text(  5, 17, "X<Y",  QPen::darkBlue, 12.0));
  Texts.append(new Text(  5, -3, "X>Y", QPen::darkBlue, 12.0));
  Texts.append(new Text(  5,-23, "X=Y", QPen::darkBlue, 12.0));

  Ports.append(new Port(-60,-10));  // X0
  Ports.append(new Port(-60, 10));  // X1
  Ports.append(new Port(-60, 30));  // Y0
  Ports.append(new Port(-60, 50));  // Y1
  Ports.append(new Port( 60, 30));  // L
  Ports.append(new Port( 60, 10));  // G
  Ports.append(new Port( 60,-10));  // E

  x1 = -60; y1 = -54;
  x2 =  60; y2 =  64;
}

QString comp_2bit::vhdlCode( int )
{
  QString s="";
  QString td=";\n";

  if(strtod(Props.at(1)->Value.latin1(), 0) != 0.0) { // delay time
    td = Props.at(1)->Value;
    if(!VHDL_Time(td, Name))
      return td;    // Time does not have VHDL format.
    td = " after " + td + ";\n";
  }

  QString X0   = Ports.at(0)->Connection->Name;
  QString X1   = Ports.at(1)->Connection->Name;
  QString Y0   = Ports.at(2)->Connection->Name;
  QString Y1   = Ports.at(3)->Connection->Name;
  QString L    = Ports.at(4)->Connection->Name;
  QString G    = Ports.at(5)->Connection->Name;
  QString E    = Ports.at(6)->Connection->Name;
 
  s = "\n  "+Name+":process ("+X0+", "+X1+", "+Y0+", "+Y1+")\n"+
      "  begin\n"+
      "    "+L+" <= ((not "+X1+") and "+Y1+") or (not ("+X1+" xor "+Y1+") and (not ("+X0+") and "+Y0+"))"+td+
      "    "+G+" <= ("+X1+" and (not "+Y1+"))"+" or (not ("+X1+" xor "+Y1+") and ("+X0+" and (not "+Y0+")))"+td+
      "    "+E+" <= (not ("+X1+" xor "+Y1+")) and (not ("+X0+" xor "+Y0+"))"+td+  
      "  end process;\n";
  return s;
}

QString comp_2bit::verilogCode( int )
{
  QString l="";
  QString td = "";

  if(strtod(Props.at(1)->Value.latin1(), 0) != 0.0) { // delay time
    td = Props.at(1)->Value;
    if(!Verilog_Time(td, Name))
      return td;    // time has not VHDL format.
    td = " #" + td ;
  }

  QString X0    = Ports.at(0)->Connection->Name;
  QString X1    = Ports.at(1)->Connection->Name;
  QString Y0    = Ports.at(2)->Connection->Name;
  QString Y1    = Ports.at(3)->Connection->Name;
  QString L     = Ports.at(4)->Connection->Name;
  QString G     = Ports.at(5)->Connection->Name;
  QString E     = Ports.at(6)->Connection->Name;

  QString LR  = "L_reg"  + Name + L;
  QString GR  = "G_reg"  + Name + G;
  QString ER  = "E_reg"  + Name + E;

  l = "\n  // "+Name+" 2bit comparator\n"+
      "  assign  "+L+" = "+LR+";\n"+
      "  reg     "+LR+" = 0;\n"+
      "  assign  "+G+" = "+GR+";\n"+
      "  reg     "+GR+" = 0;\n"+
      "  assign  "+E+" = "+ER+";\n"+
      "  reg     "+ER+" = 0;\n"+
      "  always @ ("+X0+" or "+X1+" or "+Y0+" or "+Y1+")\n"+
      "  begin\n"+
      "    "+LR+" <="+td+" ((~"+X1+") && "+Y1+") || (~("+X1+" ^ "+Y1+") && (~("+X0+") && "+Y0+"));\n"+
      "    "+GR+" <="+td+" ("+X1+" && (~"+Y1+"))"+" || (~("+X1+" ^ "+Y1+") && ("+X0+" && (~"+Y0+")));\n"+
      "    "+ER+" <="+td+" (~("+X1+" ^ "+Y1+")) && (~("+X0+" ^ "+Y0+"));\n"+  
      "  end\n";
  
  return l;
}
