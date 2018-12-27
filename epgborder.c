/*
 * Epgborder plugin for the Video Disk Recorder
 * Copyright (C) 2018 Manuel Reimer <manuel.reimer@gmx.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <vdr/plugin.h>
#include <vdr/epg.h>

static const char *VERSION        = "1.0.0";
static const char *DESCRIPTION    = "Blocks EPG storage starting from a set border channel number";

// Global variable to store the "limit" channel number
int BorderChannelNumber = 0;



// ---------- cEpgHandler ----------

class cMyEpgHandler : public cEpgHandler {
public:
  virtual bool IgnoreChannel(const cChannel *Channel);
};

bool cMyEpgHandler::IgnoreChannel(const cChannel *Channel) {
  // Default value (0) means "unconfigured"
  if (BorderChannelNumber <= 0)
    return false;

  // Don't block channels "in front of the border channel"
  if (Channel->Number() < BorderChannelNumber)
    return false;

  // Block everything else
  return true;
}


// ---------- cMenuSetupPage ----------

class cMenuSetupEpgborder : public cMenuSetupPage {
private:
  int newBorderChannelNumber;
protected:
  virtual void Store(void);
public:
  cMenuSetupEpgborder(void);
};

cMenuSetupEpgborder::cMenuSetupEpgborder(void) {
  newBorderChannelNumber = BorderChannelNumber;
  Add(new cMenuEditIntItem(tr("Block EPG starting from channel number"), &newBorderChannelNumber));
}

void cMenuSetupEpgborder::Store(void) {
  SetupStore("BorderChannelNumber", BorderChannelNumber = newBorderChannelNumber);
}


// ---------- cPlugin ----------

class cPluginEpgborder : public cPlugin {
private:
public:
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return DESCRIPTION; }
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  virtual bool Initialize(void);
};

bool cPluginEpgborder::Initialize(void) {
  new cMyEpgHandler;
  return true;
}

cMenuSetupPage *cPluginEpgborder::SetupMenu(void) {
  return new cMenuSetupEpgborder;
}

bool cPluginEpgborder::SetupParse(const char *Name, const char *Value) {
  if (!strcasecmp(Name, "BorderChannelNumber")) BorderChannelNumber = atoi(Value);
  else
     return false;
  return true;
}


VDRPLUGINCREATOR(cPluginEpgborder); // Don't touch this!
