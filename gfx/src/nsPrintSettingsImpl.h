/* -*- Mode: IDL; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * The contents of this file are subject to the Mozilla Public
 * License Version 1.1 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 * 
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 * 
 * The Original Code is the Mozilla browser.
 * 
 * The Initial Developer of the Original Code is Netscape
 * Communications, Inc.  Portions created by Netscape are
 * Copyright (C) 1999, Mozilla.  All Rights Reserved.
 * 
 * Contributor(s):
 *   Travis Bogard <travis@netscape.com>
 */

#ifndef nsPrintSettingsImpl_h__
#define nsPrintSettingsImpl_h__

#include "nsIPrintSettings.h"  
#include "nsMargin.h"  
#include "nsString.h"
#include "nsWeakReference.h"  

#define NUM_HEAD_FOOT 3

//*****************************************************************************
//***    nsPrintSettings
//*****************************************************************************

#undef  IMETHOD_VISIBILITY
#define IMETHOD_VISIBILITY default

class NS_GFX nsPrintSettings : public nsIPrintSettings
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIPRINTSETTINGS

  nsPrintSettings();
  nsPrintSettings(const nsPrintSettings& aPS);
  virtual ~nsPrintSettings();

  virtual nsPrintSettings& operator=(const nsPrintSettings& rhs);

protected:
  // May be implemented by the platform-specific derived class                       
  virtual nsresult _Clone(nsIPrintSettings **_retval);
  virtual nsresult _Assign(nsIPrintSettings *aPS);
  
  typedef enum {
    eHeader,
    eFooter
  } nsHeaderFooterEnum;


  nsresult GetMarginStrs(PRUnichar * *aTitle, nsHeaderFooterEnum aType, PRInt16 aJust);
  nsresult SetMarginStrs(const PRUnichar * aTitle, nsHeaderFooterEnum aType, PRInt16 aJust);

  // Members
  nsWeakPtr     mSession; // Should never be touched by Clone or Assign
 
  nsMargin      mMargin;
  PRInt32       mPrintOptions;

  // scriptable data members
  PRInt16       mPrintRange;
  PRInt32       mStartPageNum; // only used for ePrintRange_SpecifiedRange
  PRInt32       mEndPageNum;
  double        mScaling;
  PRBool        mPrintBGColors;  // print background colors
  PRBool        mPrintBGImages;  // print background images

  PRInt16       mPrintFrameTypeUsage;
  PRInt16       mPrintFrameType;
  PRBool        mHowToEnableFrameUI;
  PRBool        mIsCancelled;
  PRBool        mPrintSilent;
  PRBool        mPrintPreview;
  PRBool        mShrinkToFit;
  PRBool        mShowPrintProgress;
  PRInt32       mPrintPageDelay;

  nsString      mTitle;
  nsString      mURL;
  nsString      mPageNumberFormat;
  nsString      mHeaderStrs[NUM_HEAD_FOOT];
  nsString      mFooterStrs[NUM_HEAD_FOOT];

  nsString      mPaperName;
  nsString      mPlexName;
  PRInt16       mPaperData;
  PRInt16       mPaperSizeType;
  double        mPaperWidth;
  double        mPaperHeight;
  PRInt16       mPaperSizeUnit;

  PRBool        mPrintReversed;
  PRBool        mPrintInColor; // a false means grayscale
  PRInt32       mPaperSize;    // see page size consts
  PRInt32       mOrientation;  // see orientation consts
  nsString      mPrintCommand;
  PRInt32       mNumCopies;
  nsXPIDLString mPrinter;
  PRBool        mPrintToFile;
  nsString      mToFileName;
  PRPackedBool  mIsInitedFromPrinter;
  PRPackedBool  mIsInitedFromPrefs;

};

#undef  IMETHOD_VISIBILITY
#define IMETHOD_VISIBILITY hidden

#endif /* nsPrintSettings_h__ */
