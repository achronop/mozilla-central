/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * The contents of this file are subject to the Netscape Public License
 * Version 1.0 (the "License"); you may not use this file except in
 * compliance with the License.  You may obtain a copy of the License at
 * http://www.mozilla.org/NPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See
 * the License for the specific language governing rights and limitations
 * under the License.
 *
 * The Original Code is Mozilla Communicator client code.
 *
 * The Initial Developer of the Original Code is Netscape Communications
 * Corporation.  Portions created by Netscape are Copyright (C) 1998
 * Netscape Communications Corporation.  All Rights Reserved.
 */


#include "nsISessionHistory.h"
#include "nsVoidArray.h"
//#include "nsIDocumentLoaderObserver.h"
#include "nsIWebShell.h"
#include "prmem.h"
#include "nsString.h"
#include "nsIFactory.h"

// Interface ID for nsIHistoryEntry
#define NS_IHISTORY_ENTRY_IID \
{ 0x68e73d53, 0x12eb, 0x11d3, { 0xbd, 0xc0, 0x0, 0x50, 0x4, 0xa, 0x9b, 0x44 } \
}

/* Define Class IDs */
static NS_DEFINE_IID(kWebShellCID,         NS_WEB_SHELL_CID);


/* Define Interface IDs */
static NS_DEFINE_IID(kIFactoryIID,            NS_IFACTORY_IID);
static NS_DEFINE_IID(kISessionHistoryIID,     NS_ISESSION_HISTORY_IID);
static NS_DEFINE_IID(kIHistoryEntryIID,       NS_IHISTORY_ENTRY_IID);


// Advance declarations
class nsHistoryEntry;

static nsHistoryEntry *  GenerateTree(nsIWebShell * aWebShell,nsHistoryEntry *aparent, nsISessionHistory * aSHist);

#define APP_DEBUG 0

class nsHistoryEntry
{
public:
//  static const nsIID& GetIID() { static nsIID iid = NS_IHISTORY_ENTRY_IID; return iid; }

  //ctor
  nsHistoryEntry();
  ~nsHistoryEntry();

   //nsISupports
//   NS_DECL_ISUPPORTS

  /**
   * Create the History data structures for the current URL. This method
   * will also generate the history tree for the URL if it contains frames
   */
  nsresult Create(nsIWebShell * aWebShell, nsHistoryEntry * aParent, nsISessionHistory * aSHist);

  /**
   * Load the entry in the content Area
   */
  PRBool  Load(nsHistoryEntry * aPrevEntry, PRBool aLoadingFlag);

  /**
   * Destroy the  historyentry
   */
  nsresult DestroyChildren();

  /**
   * Get the name of the page 
   */
 // nsresult GetName(const PRUnichar ** aName);

  /**
   * Set the name  of the page 
   */
 // nsresult SetName(const PRUnichar * aName);

  /**
   * Get the URL  of the page 
   */
  nsresult GetURL(const  PRUnichar ** aURL);

  /**
   * Set the URL  of the page 
   */
  nsresult SetURL(const PRUnichar * aURL);

  /**
   * Get the webshell  of the page 
   */
  nsresult GetWebShell(nsIWebShell *& aName);

  /**
   * Set the webshell  of the page 
   */
  nsresult SetWebShell(nsIWebShell * aName);

  /**
   *  Add a child
   */
  nsresult AddChild(nsHistoryEntry * aChild);

  /**
   *  Get the child count
   */
  PRInt32  GetChildCnt();

  /**
   *  Get the child at a given index
   */
  nsresult GetChildAt(PRInt32 aIndex, nsHistoryEntry *& aResult);

  /**
   *  Set parent
   */
  nsresult SetParent(nsHistoryEntry * aParent);

  /**
   *  Get parent
   */
  nsresult GetParent(nsHistoryEntry *& aResult);

  /**
   *  Set the SessionHistory ID 
   */
  nsresult SetSessionHistoryID(nsISessionHistory * aSessionHistory);

  /**
   * Get a handle to the history entry for a Webshell 
   */
  
  nsHistoryEntry * GetHistoryForWS(nsIWebShell * aWS);
  
  /**
   *  Get the handle to the root of the  document structure. If the
   * page contains a frame set, this will return handle to the
   * topmost url that contains the outermost frameset.
   */
  nsHistoryEntry *  GetRootDoc();

  /**
   * Get a handle to the node for a particular URL in a history tree
   */
  nsHistoryEntry *  GetSubTreeForURL(const PRUnichar * aURL /* , nsHistoryEntry *& aResult */);

PRBool
CompareURLs(nsHistoryEntry * prev, nsHistoryEntry * cur);

PRBool
CompareNames(nsHistoryEntry * prev, nsHistoryEntry * cur);

PRBool
CompareWebShells(nsHistoryEntry * prev, nsHistoryEntry * cur);

protected:
//   virtual ~nsHistoryEntry();

private:
  
  nsIWebShell *       mWS;    //Webshell corresponding to this history entry
  nsString *          mURL;   // URL for this history entry
  nsString *          mName;  // Name of the document
  nsVoidArray         mChildren;  //  children list
  PRInt32             mChildCount; // # of children
  nsHistoryEntry *    mParent;     
  nsISessionHistory * mHistoryList;  // The handle to the session History to 
                                    // which this entry belongs
  PRBool              mURLChanged;  // State info on whether the URL of the
                                    // history entry is the same as the
                                    // corresponding on screen
};

nsHistoryEntry::nsHistoryEntry()
{
   mURLChanged = PR_TRUE;
   mChildCount = 0;
   mWS  = nsnull;
   mHistoryList = nsnull;
   mParent = nsnull;
   mURL = nsnull;
   mName = nsnull;
 
//  NS_INIT_REFCNT();
}

nsHistoryEntry::~nsHistoryEntry()
{
   DestroyChildren();
}

//NS_IMPL_ADDREF(nsHistoryEntry)
//NS_IMPL_RELEASE(nsHistoryEntry)

#if 0
NS_IMETHODIMP
nsHistoryEntry::GetName(const PRUnichar** aName)
{
  *aName = mName->GetUnicode();
  return NS_OK;
}

NS_IMETHODIMP
nsHistoryEntry::SetName(const PRUnichar* aName)
{
  mName = new nsString(aName);
  return NS_OK;
}
#endif  /* 0 */

nsresult
nsHistoryEntry::GetURL(const PRUnichar** aURL)
{
  *aURL = mURL->GetUnicode();
  return NS_OK;
}

nsresult
nsHistoryEntry::SetURL(const PRUnichar* aURL)
{
  mURL =  new nsString(aURL);
  return NS_OK;
}

nsresult
nsHistoryEntry::GetWebShell(nsIWebShell *& aResult)
{
  aResult = mWS;
  NS_IF_ADDREF(mWS);
  return NS_OK;
}

nsresult
nsHistoryEntry::SetWebShell(nsIWebShell * aWebShell)
{
  mWS = aWebShell;
  return NS_OK;
}


PRInt32 
nsHistoryEntry::GetChildCnt()
{
  return mChildCount;
}

nsresult
nsHistoryEntry::GetChildAt(PRInt32 aIndex, nsHistoryEntry *& aResult)
{
  if (PRUint32(aIndex) >= PRUint32(mChildren.Count())) {
    aResult = nsnull;
  }
  else {
    aResult = (nsHistoryEntry*) mChildren.ElementAt(aIndex);
//    NS_IF_ADDREF(aResult);
  }
  return NS_OK;
}


nsresult
nsHistoryEntry::SetSessionHistoryID(nsISessionHistory * aHistoryListID)
{
//   NS_IF_RELEASE(mHistoryList);
   mHistoryList = aHistoryListID;
//   NS_IF_ADDREF(aHistoryListID);
   return NS_OK;
}


nsresult
nsHistoryEntry::SetParent(nsHistoryEntry* aParent)
{
//  NS_IF_RELEASE(mParent);
  mParent = aParent;
//  NS_IF_ADDREF(aParent);
  return NS_OK;
}



nsresult
nsHistoryEntry::GetParent(nsHistoryEntry *& aParent)
{

  aParent = mParent;
//  NS_IF_ADDREF(mParent);
  return NS_OK;
}

nsresult
nsHistoryEntry::AddChild(nsHistoryEntry* aChild)
{
  NS_PRECONDITION(nsnull != aChild, "null ptr");
  if (nsnull == aChild) {
    return NS_ERROR_NULL_POINTER;
  }
  mChildren.AppendElement(aChild);
  mChildCount++;
  aChild->SetParent(this);
//  NS_ADDREF(aChild);

  return NS_OK;
}

  
nsresult
nsHistoryEntry::DestroyChildren() {
   nsHistoryEntry * hEntry=nsnull;
   nsHistoryEntry * parent=nsnull;

  PRInt32 i, n;

  n = mChildren.Count();
  for (i = 0; i < n; i++) {
    nsHistoryEntry* child = (nsHistoryEntry *) mChildren.ElementAt(i);
    child->SetParent(nsnull);
    delete child;
//    NS_RELEASE(child);
    mChildCount--;
  }
  mChildren.Clear();
  return NS_OK;

}


/* Create a historyentry for the given webshell. If the
 * webshell has children, this method recursively goes down
 * the tree and creates entries for the children too.
 */

nsresult 
nsHistoryEntry::Create(nsIWebShell * aWebShell, nsHistoryEntry * aParent, nsISessionHistory * aSHist) {

   nsHistoryEntry * hEntry = nsnull;
   const PRUnichar * name = nsnull;
   const PRUnichar * url = nsnull;


   // Get the  webshell's url.
   aWebShell->GetURL(&url);
   // save the webshell's URL in the history entry
   SetURL(url);

   //Get the  webshell's name
//   aWebShell->GetName(&name);
//   SetName(name);

   //Save the webshell id
   SetWebShell(aWebShell);
   
   if (APP_DEBUG) printf("SessionHistory::Create Creating Historyentry %x  for webshell %x, parent entry = %x\n", this, aWebShell, aParent);

   if (aParent)
     aParent->AddChild(this);

   // Save the handle to the window's history list   
   SetSessionHistoryID(aSHist);
   return NS_OK;
}

static nsHistoryEntry *  
GenerateTree(nsIWebShell * aWebShell, nsHistoryEntry * aParent, nsISessionHistory * aSHist) {

   nsHistoryEntry * hEntry = nsnull;

   hEntry = new nsHistoryEntry;

   if (!hEntry) {
      NS_ASSERTION(PR_FALSE, "Could not create a History Entry. Out of memory");
      return (nsHistoryEntry *)nsnull; 
   }

   hEntry->Create(aWebShell, aParent, aSHist);

  // If the webshell has children, go thro' the child list and create 
  // the history tree for the children recursively.

  PRInt32  cnt = 0;
  aWebShell->GetChildCount(cnt);
  if (cnt > 0) {
    for (int i=0; i<cnt; i++) {
      nsIWebShell * childWS = nsnull;
      nsHistoryEntry * hChild = nsnull;

      aWebShell->ChildAt(i, childWS); 
      if (childWS) {
        hChild = new nsHistoryEntry;
        hChild->Create(childWS, hEntry, aSHist);
      }
    }
  }
  return hEntry;

}



/* Get the historyentry corresponding to a URL */
nsHistoryEntry *
nsHistoryEntry::GetSubTreeForURL( const PRUnichar * aURL) {
  nsString    cURL="";
  const PRUnichar * str = nsnull;
  nsHistoryEntry * result=nsnull;

  /* Get the url string for the current entry */
  GetURL(&str);
  cURL = str;
  
  if (cURL == aURL) {
     return this;
  }

  PRInt32   cnt = GetChildCnt();
  if (cnt > 0) {
    for(int i=0; i<cnt; i++) {
      nsHistoryEntry * child = nsnull;

      GetChildAt(i, child);
      if (child) {
        result = child->GetSubTreeForURL(aURL);
      }
      if (result)
        break;    
    }
  }

  return (nsHistoryEntry *) result;

}  /* Get subtree for URL */


/* Get the historyentry corresponding to a WebShell */
nsHistoryEntry *
nsHistoryEntry::GetHistoryForWS(nsIWebShell * aWebShell) {
  nsIWebShell * cWS = nsnull;
  nsHistoryEntry * result = nsnull;


  /* Get the webshell  for the current entry */
  GetWebShell(cWS);

  if (cWS == aWebShell) {
     return this;
  }

  PRInt32   cnt = GetChildCnt();
  if (cnt > 0) {
    for(int i=0; i<cnt; i++) {

      nsHistoryEntry * child = nsnull;
      GetChildAt(i, child);
      if (child) {
        result = child->GetHistoryForWS(aWebShell);
      }
      if (result)
        break;    
    }
  }

  return (nsHistoryEntry *) result;

}  /* Get subtree for WS */


/* Load the history item in the window */
PRBool 
nsHistoryEntry::Load(nsHistoryEntry * aPrevEntry, PRBool aLoadingFlag) {

   nsHistoryEntry * prev=nsnull, * cur=nsnull;
   PRBool wShellChanged=PR_FALSE, urlChanged = PR_FALSE, nameChanged=PR_FALSE;
   int i = 0; 
   nsIWebShell * pWS = nsnull, *cWS=nsnull;
   const PRUnichar * cURL = nsnull;
   PRBool result = PR_FALSE;
   nsString  cSURL="", pSURL="";


   cur = this;
   prev = aPrevEntry;

#ifdef debug_RADHA
   if (prev) {
     const PRUnichar * url;
     aPrevEntry->GetURL(&url);
     pSURL = url;

   }
   if (cur) {
     const PRUnichar * url;
     GetURL(&url);
     cSURL = url;

   }
#endif  /* debug_RADHA */
     
  // Not sure if this is the right thing to do
//   NS_ADDREF(aPrevEntry);

   if (!cur || !prev) {
     printf("SessionHistory::Load cur or prev is null, cur = %x, prev = %x\n", cur, prev);
     return NS_ERROR_NULL_POINTER;
   }

   urlChanged = CompareURLs(prev, cur);

   /* Get the webshell where the url needs to be loaded */
   prev->GetWebShell(pWS);

   /* The URL to be loaded in it */
   GetURL(&cURL);

   if (urlChanged) {

      if (APP_DEBUG) printf("SessionHistory::Load Loading URL %s in webshell %x\n", mURL->ToNewCString(), pWS);
      if (pWS)      
        pWS->LoadURL(cURL);
      return PR_TRUE;
   }
   else if (!urlChanged ) {
       /* Mark the changed flag to false. This is used in the end to determine
        * whether we are done with the whole loading process for this history
        */
       if (APP_DEBUG) printf("SessionHistory::Load URLs in webshells %x & %x match \n", mWS, pWS);
       mURLChanged = PR_FALSE;
   }
   /* Make sure the child windows are in par */

   PRInt32  cnt1=0, cnt2=0;
   cnt1 = cur->GetChildCnt();
   cnt2 = prev->GetChildCnt();

   /* If the current entry to be loaded and the one on page don't have
    * the same # of children, maybe the one on screen is is in the process of
    * building. Don't compare the children.
    */
   if (cnt1 != cnt2) {
     return PR_TRUE;
   }


   for ( i=0; i<cnt1; i++){
      nsIWebShell * cws=nsnull, *pws=nsnull;
      nsHistoryEntry * pChild=nsnull, *cChild=nsnull;

      cur->GetChildAt(i, cChild);
      prev->GetChildAt(i, pChild);
      cChild->GetWebShell(cws);
      pChild->GetWebShell(pws);
      if (APP_DEBUG) printf("SessionHistory::Load. Inside childloop Comparing webshells %x & %x\n", cws, pws);
      result = cChild->Load(pChild, PR_FALSE);
      if (result)
         break;
   }
 
   // Not sure if this is the right thing to do
//   NS_IF_RELEASE(aPrevEntry);

     return result;
}  /* Load */

PRBool
nsHistoryEntry::CompareURLs(nsHistoryEntry * prev, nsHistoryEntry * cur) {

   const PRUnichar *  pURL=nsnull, * cURL=nsnull;   
   nsString pSURL="", cSURL="";

   PRBool val=PR_FALSE;

   //Get handles to the webshell of the previous & current doc
   prev->GetURL(&pURL);
   cur->GetURL(&cURL);
  
   pSURL = pURL;
   /* For Testinf purposes */
   cSURL = cURL;

   if (pSURL == cURL)
     val = PR_FALSE;
   else
     val = PR_TRUE;

   return val;
}

nsHistoryEntry *
nsHistoryEntry::GetRootDoc(void) {

  nsHistoryEntry * top= this;

  for (;;) {
    nsHistoryEntry* parent;
    top->GetParent(parent);
    if (parent == nsnull)
      break;
    top = parent;
  }
//     NS_ADDREF(top);
     return top;
}


class nsSessionHistory: public nsISessionHistory
{

public:
   nsSessionHistory();

   //nsISupports
   NS_DECL_ISUPPORTS

  //nsISessionHistory interfaces


  /**
   * Go forward in history 
   */
  NS_IMETHOD Forward();

  /**
   * Go Back in History
   */
  NS_IMETHOD Back();

  /**
   * whether you can go forward in History
   */
  NS_IMETHOD canForward(PRBool &aResult);

  /**
   * whether you can go back in History
   */
  NS_IMETHOD canBack(PRBool &aResult);

  /**
   * Add a new URL to the History List
   */
  NS_IMETHOD add(nsIWebShell * aWebShell);

  /**
   * Go to a particular point in the history list
   */
  NS_IMETHOD  Goto(PRInt32 aHistoryIndex);

  /**
   * Get the length of the History list
   */
  NS_IMETHOD getHistoryLength(PRInt32 & aResult);

  /**
   * Get the index of the current document in the history list
   */
  NS_IMETHOD getCurrentIndex(PRInt32 & aResult);

  /**
   * Make a clone of the Session History. This will 
   * make a deep copy of the Session history data structures. 
   * This is used when you create a navigator window from the
   * current browser window
   */
//  NS_IMETHOD cloneHistory(nsISessionHistory * aSessionHistory);

  /**
   * Set the flag whether a history entry is in the middle of loading a
   * doc. See comments below for details
   */
  NS_IMETHOD SetLoadingFlag(PRBool aFlag);

  /**
   * Set the historyentry that is in the middle of loading a doc
   */
  NS_IMETHOD SetLoadingHistoryEntry(nsHistoryEntry * aHistoryEntry);


protected:

   ~nsSessionHistory();

private:
  PRInt32       mHistoryLength;
  PRInt32       mHistoryCurrentIndex;
  nsVoidArray   mHistoryEntries;
//  PRBool        mIsBackOrForward;

  /** Following  member is used to identify whether we are in the
   *  middle of loading a history document. The mIsLoadingDoc flag is 
   *  used to determine whether the document that is curently  loaded 
   *  in the window s'd go to the  end of the historylist or to be 
   *  handed over to the current history entry (mIndexOfHistoryInLoad)
   *  that is in the process of loading. The current history entry 
   *  being loaded uses this new historyentry to decide whether the 
   *  document is completely in par with the one in history. If not, it
   *  will initiate further loads. When the document currently loaded is
   *  completely on par with the one in history, it will clear the
   *  mIsLoadingDoc flag. Any new URL loaded from then on, will go to the
   *  end of the history list. Note: these members  are static.
   */ 
   PRBool            mIsLoadingDoc;
   nsHistoryEntry *  mHistoryEntryInLoad;

  /*
   * Following member is used as a  interim history entry. When we are in the
   * process of loading a history entry, with frames, we need a place holder
   * to hold the page that is loading,until we are sure that the one on screen
   * exactly matches the one in history. 
   */
  nsHistoryEntry *  mInterimPage;

};


nsSessionHistory::nsSessionHistory()
{
                  mHistoryLength = 0;
                  mHistoryCurrentIndex = -1;
                  mIsLoadingDoc = 0;
                  mHistoryEntryInLoad = (nsHistoryEntry *) nsnull; 
                  mInterimPage = (nsHistoryEntry *) nsnull;
                
  NS_INIT_REFCNT();
}


nsSessionHistory::~nsSessionHistory()
{

  // Delete all HistoryEntries

  for(int i=0; i<mHistoryLength; i++) {
     nsHistoryEntry * hEntry = (nsHistoryEntry *)mHistoryEntries[i];
     if (hEntry)
        delete hEntry;
     //NS_IF_RELEASE(hEntry);
  }
  mHistoryLength=0;
  mHistoryCurrentIndex=-1;

}

//ISupports...
NS_IMPL_ISUPPORTS(nsSessionHistory, kISessionHistoryIID);

/**
  * Called to a new page is visited either through link click or
  * by typing in the urlbar.
  */
NS_IMETHODIMP
nsSessionHistory::add(nsIWebShell * aWebShell)
{

   nsresult  rv = NS_OK;
   nsHistoryEntry * hEntry = nsnull;
   nsIWebShell * parent = nsnull;

   if (!aWebShell) {
      return NS_ERROR_NULL_POINTER;
   }

   aWebShell->GetParent(parent);

   if (!parent) {
     /* This is a top level webshell. Create a new entry and
      * add to the list. 
      */
    
     hEntry = new nsHistoryEntry();
     if (!hEntry) {
       NS_ASSERTION(PR_FALSE, "nsSessionHistory::add Low memory");
       return NS_ERROR_OUT_OF_MEMORY;
     }
     hEntry->Create(aWebShell, nsnull, this);


     /* Check if we are in the middle of loading a history doc.
      * If so, pass on the newly created object to the currently
      * loading history entry for comparison purposes.
      */

      if (mIsLoadingDoc) {
        PRBool ret=PR_TRUE;

#if 0
        /* Remove any old interim page that is hanging around */
        if (mInterimPage) {
          delete mInterimPage;
          mInterimPage = nsnull;
        }
#endif   /* 0 */

          mInterimPage = hEntry;
          PRInt32 cnt1=0, cnt2=0;
   
          if (mHistoryEntryInLoad)
             cnt1 = mHistoryEntryInLoad->GetChildCnt();
          cnt2 = mInterimPage->GetChildCnt();

          /* The interim page that is being built doesn't quite have
           * all the  children yet and therefore not ready for comparison.
           * So return. When it has all the children, compare it 
           * with the one in history by calling Load()
           */

          if (cnt1  != cnt2)
            return NS_OK;


        ret = mHistoryEntryInLoad->Load(mInterimPage, mIsLoadingDoc);
        if (!ret) {
          /* The interim page matches exactly with the one in history.
           * So, don't need to keep it around. Clear all flags and
           * return.
           */
   
          mIsLoadingDoc =  PR_FALSE;      
          mHistoryEntryInLoad = (nsHistoryEntry *)nsnull;

          delete mInterimPage;
          mInterimPage = (nsHistoryEntry *) nsnull;
          return NS_OK;
        }
      }

     /* Set the flag in webshell that indicates that it has been
      * added to session History 
      */
     aWebShell->SetIsInSHist(PR_TRUE);

   if ((mHistoryLength - (mHistoryCurrentIndex+1)) > 0) {
      /* We are somewhere in the middle of the history and a
       * new page was visited. Purge all entries from the current index
       * till the end of the list and append the current page to the
       * list
       */

       for(int i=mHistoryLength-1; i>mHistoryCurrentIndex; i--) {
          nsHistoryEntry * hEntry = (nsHistoryEntry *)mHistoryEntries.ElementAt(i);
          //NS_IF_RELEASE(hEntry);
          delete hEntry;
          mHistoryEntries.RemoveElementAt(i);
          mHistoryLength--;
       }
   }

   mHistoryEntries.AppendElement((void *)hEntry);
   mHistoryLength++;
   mHistoryCurrentIndex++;        
   return NS_OK;

   }
   else {
      /* This is a frame webshell. Check if it is a new frame. If so,
       * append to the existing history entry. Else, create a 
       * new tree to record the change in URL 
       */
     
      PRBool inSHist = PR_TRUE;
   
      aWebShell->GetIsInSHist(inSHist);

      if (!inSHist) {
         /* This is a newly created frame. Add it to the current entry */
         nsHistoryEntry * curEntry=nsnull, * newEntry=nsnull, 
                        * parentEntry=nsnull;

         if (mIsLoadingDoc) {
           /* We are in the middle of loading a history entry. The
            * newly created frame must be a child of this interimPage.
            */
           if(mInterimPage)
             curEntry = mInterimPage;
         }
         else {
  
           // Get a handle to the current History entry
           curEntry = (nsHistoryEntry *) mHistoryEntries.ElementAt(mHistoryCurrentIndex);
         }
        // Get a handle to the parent of the new frame WS;
        nsIWebShell * parentWS = nsnull;
        aWebShell->GetParent(parentWS);
   
        // Get the history entry corresponding to the parentWS
        if (curEntry)
          parentEntry = curEntry->GetHistoryForWS(parentWS);

        // Create a new HistoryEntry for the frame & init values
        newEntry = new nsHistoryEntry();
        if (!newEntry) {
           NS_ASSERTION(PR_FALSE, "nsSessionHistory::add Low memory");
           return NS_ERROR_OUT_OF_MEMORY;
        }
        newEntry->Create(aWebShell, parentEntry, this);
        aWebShell->SetIsInSHist(PR_TRUE);
        
        if (mIsLoadingDoc) {
          /* Pass on the interim page to Load() to see if we are done */
          PRBool ret=PR_TRUE;
          
          PRInt32 cnt1=0, cnt2=0;
   
		  if (mHistoryEntryInLoad)
             cnt1 = mHistoryEntryInLoad->GetChildCnt();
		  if (mInterimPage)
             cnt2 = mInterimPage->GetChildCnt();

          /* The interim page that is being built doesn't quite have
           * all the  children yet and therefore not ready for comparison.
           * So return. When it has all the children, compare it 
           * with the one in history by calling Load()
           */

          if (cnt1  != cnt2)
            return NS_OK;

		  if (mHistoryEntryInLoad)
             ret = mHistoryEntryInLoad->Load(mInterimPage, mIsLoadingDoc);
          if (!ret) {
            /* The interim page matches exactly with the one in history.
             * So, replace the current entry in history with the interim page,
             * Clear all flags and return.
             */
            
            PRInt32 index = mHistoryEntries.IndexOf((void *) mHistoryEntryInLoad);
            if ((index >= 0) && (index < mHistoryLength)) {
               mHistoryEntries.ReplaceElementAt( mInterimPage, index);

               mIsLoadingDoc =  PR_FALSE;      
               delete mHistoryEntryInLoad;
               mHistoryEntryInLoad = (nsHistoryEntry *)nsnull;

               mInterimPage = nsnull;
               return NS_OK;
            }
          }        
        }
        else
          return NS_OK;
      }
      else {  /* matches if (!inSHist) */
         /* This page is already in Session History. That means that
          * the URL changed in a existing Frame. Create a whole new tree
          * representing the current state of the document and add it to the
          * end of the history list
          */

          nsIWebShell * root = nsnull;
          nsHistoryEntry * newEntry = nsnull;

          aWebShell->GetRootWebShell(root);
  
          if (root)   
            newEntry = GenerateTree(root, nsnull, this);

          if (newEntry && mIsLoadingDoc) {
             PRBool result = PR_FALSE;
             /* We are currently in the process of loading a history entry.
              * Pass this entry to Load() to figure if what we have in screen
              * matches with what we have in history
              */
			 if (mHistoryEntryInLoad)
                result = mHistoryEntryInLoad->Load(newEntry, mIsLoadingDoc);
             /* We don't need the tree anymore. We don't quite care at this
              * thime if the Load returned True or False. Irrespective of
              * what happened, if we reload a page in Load() we will
              * generate the complete tree when we get here. Probably a candidate
              * for improvement if going back& forward in frames really 
              * really suck.
              */
             if (!result) {
                if (mInterimPage) {
                    PRInt32 index = mHistoryEntries.IndexOf((void *) mHistoryEntryInLoad);
                    if ((index >= 0) && (index < mHistoryLength)) {
                       mHistoryEntries.ReplaceElementAt(newEntry, index);
                       delete mHistoryEntryInLoad;
                       delete mInterimPage;
                       mInterimPage = nsnull;
                       mIsLoadingDoc =  PR_FALSE;      
                       mHistoryEntryInLoad = (nsHistoryEntry *)nsnull;
                       return NS_OK;
                    }
                }  // mInterimPage

                mIsLoadingDoc =  PR_FALSE;      
                mHistoryEntryInLoad = (nsHistoryEntry *)nsnull;

             }   // !result
             delete newEntry;    
             newEntry = nsnull;
             return ((PRInt32)PR_TRUE);

          } // (newEntry && mIsLoadingDoc)

          if (newEntry) {
            if ((mHistoryLength - (mHistoryCurrentIndex+1)) > 0) {
            /* We are somewhere in the middle of the history and a
             *  new page was visited. Purge all entries from the current index
             * till the end of the list and append the current page to the
             * list
             */

             for(int i=mHistoryLength-1; i>mHistoryCurrentIndex; i--) {
                 nsHistoryEntry * hEntry = (nsHistoryEntry *)mHistoryEntries.ElementAt(i);
                 //NS_IF_RELEASE(hEntry);
                 delete hEntry;
                 mHistoryEntries.RemoveElementAt(i);
                 mHistoryLength--;
              }
            }
            mHistoryEntries.AppendElement((void *)newEntry);
            mHistoryLength++;
            mHistoryCurrentIndex++;        
            return NS_OK;
        }  //newEntry
     }   //else 
   }  //else for !aParent
   return NS_OK;
}  //add


NS_IMETHODIMP
nsSessionHistory::Goto(PRInt32 aGotoIndex)
{
   nsresult rv = NS_OK;
   int prevIndex = 0;

   if ((aGotoIndex < 0) ||  (aGotoIndex >= mHistoryLength))
      return NS_ERROR_NULL_POINTER;

   prevIndex = mHistoryCurrentIndex;
   nsHistoryEntry * hPrevEntry=nsnull, * hCurrentEntry = nsnull;

   //get a handle to the current page to be passed to nsHistoryEntry
   // as the previous page.
   hPrevEntry = (nsHistoryEntry *)mHistoryEntries.ElementAt(prevIndex);
   hCurrentEntry = (nsHistoryEntry *) mHistoryEntries.ElementAt(aGotoIndex);

   // Set the flag that indicates that we are currently
   // traversing with it the history
   mIsLoadingDoc = PR_TRUE;
   mHistoryEntryInLoad = hCurrentEntry;

  //Load the page
  if (hCurrentEntry != nsnull)
     rv = hCurrentEntry->Load(hPrevEntry, mIsLoadingDoc);

#if 0
  //Update the current & previous indices accordingly.
  if (!rv)
    mHistoryCurrentIndex = aGotoIndex;
  else
    mHistoryCurrentIndex = prevIndex;
#endif

   mHistoryCurrentIndex = aGotoIndex;
   return rv;
}

NS_IMETHODIMP
nsSessionHistory::Back()
{
   nsresult rv = NS_OK;
   nsHistoryEntry * hEntry=nsnull;

   if (mHistoryCurrentIndex <= 0) 
      return NS_ERROR_NULL_POINTER;

   Goto(mHistoryCurrentIndex -1);
   return NS_OK;

}

NS_IMETHODIMP
nsSessionHistory::Forward()
{
   nsresult rv = NS_OK;
   PRInt32  prevIndex = 0;

   if (mHistoryCurrentIndex == mHistoryLength-1)
      return NS_ERROR_NULL_POINTER;

   Goto(mHistoryCurrentIndex+1);
   return NS_OK;

}

NS_IMETHODIMP
nsSessionHistory::SetLoadingFlag(PRBool aFlag)
{
  mIsLoadingDoc = aFlag;
  return NS_OK;
}

NS_IMETHODIMP
nsSessionHistory::SetLoadingHistoryEntry(nsHistoryEntry *  aHistoryEntry)
{
  mHistoryEntryInLoad  = aHistoryEntry;
  return NS_OK;
}

NS_IMETHODIMP
nsSessionHistory::canForward(PRBool & aResult)
{
   if (mHistoryCurrentIndex == mHistoryLength-1)
      aResult = PR_FALSE;
   else
      aResult = PR_TRUE;
   return NS_OK;
}

NS_IMETHODIMP
nsSessionHistory::canBack(PRBool & aResult)
{
   if (mHistoryCurrentIndex == 0)
      aResult = PR_FALSE;
   else
      aResult = PR_TRUE;
   return NS_OK;
}

NS_IMETHODIMP
nsSessionHistory::getHistoryLength(PRInt32 & aResult)
{
   aResult=mHistoryLength;
   return NS_OK;
}


NS_IMETHODIMP
nsSessionHistory::getCurrentIndex(PRInt32 & aResult)
{
   aResult = mHistoryCurrentIndex;
   return NS_OK;
}


NS_EXPORT nsresult NS_NewSessionHistory(nsISessionHistory** aResult)
{
  if (nsnull == aResult) {
    return NS_ERROR_NULL_POINTER;
  }

  *aResult = new nsSessionHistory();
  if (nsnull == *aResult) {
    return NS_ERROR_OUT_OF_MEMORY;
  }

  NS_ADDREF(*aResult);
  return NS_OK;
}

#if 0
NS_IMETHODIMP
cloneHistory(nsISessionHistory * aSessionHistory) {

   return NS_OK;
}
#endif  /* 0 */

//----------------------------------------------------------------------

// Factory code for creating nsSessionHistory

class nsSessionHistoryFactory : public nsIFactory
{
public:
  nsSessionHistoryFactory();
  NS_DECL_ISUPPORTS

  // nsIFactory methods
  NS_IMETHOD CreateInstance(nsISupports *aOuter,
                            const nsIID &aIID,
                            void **aResult);
  
  NS_IMETHOD LockFactory(PRBool aLock);  
protected:
  virtual ~nsSessionHistoryFactory();
};


nsSessionHistoryFactory::nsSessionHistoryFactory()
{
  NS_INIT_REFCNT();
}

nsresult
nsSessionHistoryFactory::LockFactory(PRBool lock)
{

  return NS_OK;
}

nsSessionHistoryFactory::~nsSessionHistoryFactory()
{
  NS_ASSERTION(mRefCnt == 0, "non-zero refcnt at destruction");
}

NS_IMPL_ISUPPORTS(nsSessionHistoryFactory, kIFactoryIID);


nsresult
nsSessionHistoryFactory::CreateInstance(nsISupports *aOuter,
                                  const nsIID &aIID,
                                  void **aResult)
{
  nsresult rv;
  nsSessionHistory* inst;

  if (aResult == NULL) {
    return NS_ERROR_NULL_POINTER;
  }
  *aResult = NULL;
  if (nsnull != aOuter) {
    rv = NS_ERROR_NO_AGGREGATION;
    goto done;
  }


  NS_NEWXPCOM(inst, nsSessionHistory);
  if (inst == NULL) {
    rv = NS_ERROR_OUT_OF_MEMORY;
    goto done;
  }

  NS_ADDREF(inst);
  rv = inst->QueryInterface(aIID, aResult);
  NS_RELEASE(inst);

done:
  return rv;
}


extern "C" NS_APPSHELL nsresult
NS_NewSessionHistoryFactory(nsIFactory** aFactory)
{
  nsresult rv = NS_OK;

  nsIFactory* inst = new nsSessionHistoryFactory();
  if (nsnull == inst) {
    rv = NS_ERROR_OUT_OF_MEMORY;
  }
  else {
    NS_ADDREF(inst);
  }
  *aFactory = inst;
  return rv;
}

#if 0
PRBool
nsHistoryEntry::CompareWebShells(nsHistoryEntry * prev, nsHistoryEntry * cur) {

   nsIWebShell * pWS, *cWS;
   PRBool val=PR_FALSE;

   //Get handles to the webshell of the previous & current doc
   prev->GetWebShell(pWS);
   cur->GetWebShell(cWS);

   if (pWS == cWS)
     val = PR_FALSE;
   else
     val = PR_TRUE;

   return val;
}
#endif


#if 0
PRBool
nsHistoryEntry::CompareNames(nsHistoryEntry * prev, nsHistoryEntry * cur) {

   nsIUnichar *  pname, * cName;   
   nsString pSName,cSName;

   PRBool val=PR_FALSE;

   //Get handles to the webshell of the previous & current doc
   prev->GetName(&pName);
   cur->GetName(&cName);
  
   pSName = pName;
   cSName = cName;

   if (pSName.equals(cSName)
     val = PR_FALSE;
   else
     val = PR_TRUE;

   return val;
}
#endif
