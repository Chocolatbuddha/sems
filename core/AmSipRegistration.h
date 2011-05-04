/*
 * Copyright (C) 2006 iptego GmbH
 * Copyright (C) 2011 Stefan Sayer
 *
 * This file is part of SEMS, a free SIP media server.
 *
 * SEMS is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. This program is released under
 * the GPL with the additional exemption that compiling, linking,
 * and/or using OpenSSL is allowed.
 *
 * For a license to use the SEMS software under conditions
 * other than those described here, or to purchase support for this
 * software, please contact iptel.org by e-mail at the following addresses:
 *    info@iptel.org
 *
 * SEMS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, write to the Free Software 
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _AmSipRegistration_h_
#define _AmSipRegistration_h_
#include <string>
using std::string;

#include "ampi/SIPRegistrarClientAPI.h"
#include "ampi/UACAuthAPI.h"
#include "AmUriParser.h"
#include "AmSessionEventHandler.h"

#define REGISTER_SEND_TIMEOUT 60 

struct SIPRegistrationInfo {
  string domain;
  string user;
  string name;
  string auth_user;
  string pwd;
  string proxy;
  string contact;

  SIPRegistrationInfo(const string& domain,
		      const string& user,
		      const string& name,
		      const string& auth_user,
		      const string& pwd,
		      const string& proxy,
		      const string& contact)
    : domain(domain),user(user),name(name),
    auth_user(auth_user),pwd(pwd),proxy(proxy),contact(contact)
  { }
};

class SIPRegistration : public AmSipDialogEventHandler,
			public DialogControl,
			public CredentialHolder
	
{
	
  AmSipDialog dlg;
  UACAuthCred cred;

  SIPRegistrationInfo info;

  // session to post events to 
  string sess_link;      

  AmSessionEventHandler* seh;

  AmSipRequest req;

  AmUriParser server_contact;
  AmUriParser local_contact;

  time_t reg_begin;	
  unsigned int reg_expires;
  time_t reg_send_begin; 

 public:
  SIPRegistration(const string& handle,
		  const SIPRegistrationInfo& info,
		  const string& sess_link);
  ~SIPRegistration();

  void setSessionEventHandler(AmSessionEventHandler* new_seh);

  void doRegistration();
  void doUnregister();
	
  bool timeToReregister(time_t now_sec);
  bool registerExpired(time_t now_sec);
  void onRegisterExpired();
  void onRegisterSendTimeout();

  bool registerSendTimeout(time_t now_sec);

  void onSendRequest(const string& method,
		     const string& content_type,
		     const string& body,
		     string& hdrs,
		     int flags,
		     unsigned int cseq);
	
  void onSendReply(const AmSipRequest& req,
		   unsigned int  code,
		   const string& reason,
		   const string& content_type,
		   const string& body,
		   string& hdrs,
		   int flags);

  // DialogControl if
  AmSipDialog* getDlg() { return &dlg; }
  // CredentialHolder	
  UACAuthCred* getCredentials() { return &cred; }

  void onSipReply(const AmSipReply& reply, int old_dlg_status, const string& trans_method);
  void onSipRequest(const AmSipRequest& req) {}
  void onInvite2xx(const AmSipReply&) {}
  void onNoAck(unsigned int) {}
  void onNoPrack(const AmSipRequest &, const AmSipReply &) {}
  void onInvite1xxRel(const AmSipReply &){}
  void onPrack2xx(const AmSipReply &){}
  void onFailure(AmSipDialogEventHandler::FailureCause cause, 
      const AmSipRequest*, const AmSipReply*){}

  /** is this registration registered? */
  bool active; 
  /** should this registration be removed from container? */
  bool remove;
  /** are we waiting for the response to a register? */
  bool waiting_result;

  enum RegistrationState {
    RegisterPending = 0,
    RegisterActive,
    RegisterExpired
  };
  /** return the state of the registration */
  RegistrationState getState(); 
  /** return the expires left for the registration */
  unsigned int getExpiresLeft(); 

  SIPRegistrationInfo& getInfo() { return info; }
  const string& getEventSink() { return sess_link; }
};



#endif
