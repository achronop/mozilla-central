/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* DOM object representing values in DOM computed style */

#include "nsROCSSPrimitiveValue.h"

#include "nsPresContext.h"
#include "nsStyleUtil.h"
#include "nsDOMCSSRGBColor.h"
#include "nsIDOMRect.h"
#include "nsDOMClassInfoID.h" // DOMCI_DATA
#include "nsIURI.h"
#include "nsError.h"

nsROCSSPrimitiveValue::nsROCSSPrimitiveValue()
  : mType(CSS_PX)
{
  mValue.mAppUnits = 0;
}


nsROCSSPrimitiveValue::~nsROCSSPrimitiveValue()
{
  Reset();
}

NS_IMPL_ADDREF(nsROCSSPrimitiveValue)
NS_IMPL_RELEASE(nsROCSSPrimitiveValue)


DOMCI_DATA(ROCSSPrimitiveValue, nsROCSSPrimitiveValue)

// QueryInterface implementation for nsROCSSPrimitiveValue
NS_INTERFACE_MAP_BEGIN(nsROCSSPrimitiveValue)
  NS_INTERFACE_MAP_ENTRY(nsIDOMCSSPrimitiveValue)
  NS_INTERFACE_MAP_ENTRY(nsIDOMCSSValue)
  NS_INTERFACE_MAP_ENTRY(nsISupports)
  NS_DOM_INTERFACE_MAP_ENTRY_CLASSINFO(ROCSSPrimitiveValue)
NS_INTERFACE_MAP_END


// nsIDOMCSSValue


NS_IMETHODIMP
nsROCSSPrimitiveValue::GetCssText(nsAString& aCssText)
{
  nsAutoString tmpStr;
  aCssText.Truncate();
  nsresult result = NS_OK;

  switch (mType) {
    case CSS_PX :
      {
        float val = nsPresContext::AppUnitsToFloatCSSPixels(mValue.mAppUnits);
        tmpStr.AppendFloat(val);
        tmpStr.AppendLiteral("px");
        break;
      }
    case CSS_IDENT :
      {
        AppendUTF8toUTF16(nsCSSKeywords::GetStringValue(mValue.mKeyword),
                          tmpStr);
        break;
      }
    case CSS_STRING :
    case CSS_COUNTER : /* FIXME: COUNTER should use an object */
      {
        tmpStr.Append(mValue.mString);
        break;
      }
    case CSS_URI :
      {
        if (mValue.mURI) {
          nsAutoCString specUTF8;
          mValue.mURI->GetSpec(specUTF8);

          tmpStr.AssignLiteral("url(");
          nsStyleUtil::AppendEscapedCSSString(NS_ConvertUTF8toUTF16(specUTF8),
                                              tmpStr);
          tmpStr.AppendLiteral(")");
        } else {
          // http://dev.w3.org/csswg/css3-values/#attr defines
          // 'about:invalid' as the default value for url attributes,
          // so let's also use it here as the default computed value
          // for invalid URLs.
          tmpStr.Assign(NS_LITERAL_STRING("url(about:invalid)"));
        }
        break;
      }
    case CSS_ATTR :
      {
        tmpStr.AppendLiteral("attr(");
        tmpStr.Append(mValue.mString);
        tmpStr.Append(PRUnichar(')'));
        break;
      }
    case CSS_PERCENTAGE :
      {
        tmpStr.AppendFloat(mValue.mFloat * 100);
        tmpStr.Append(PRUnichar('%'));
        break;
      }
    case CSS_NUMBER :
      {
        tmpStr.AppendFloat(mValue.mFloat);
        break;
      }
    case CSS_RECT :
      {
        NS_ASSERTION(mValue.mRect, "mValue.mRect should never be null");
        NS_NAMED_LITERAL_STRING(comma, ", ");
        nsCOMPtr<nsIDOMCSSPrimitiveValue> sideCSSValue;
        nsAutoString sideValue;
        tmpStr.AssignLiteral("rect(");
        // get the top
        result = mValue.mRect->GetTop(getter_AddRefs(sideCSSValue));
        if (NS_FAILED(result))
          break;
        result = sideCSSValue->GetCssText(sideValue);
        if (NS_FAILED(result))
          break;
        tmpStr.Append(sideValue + comma);
        // get the right
        result = mValue.mRect->GetRight(getter_AddRefs(sideCSSValue));
        if (NS_FAILED(result))
          break;
        result = sideCSSValue->GetCssText(sideValue);
        if (NS_FAILED(result))
          break;
        tmpStr.Append(sideValue + comma);
        // get the bottom
        result = mValue.mRect->GetBottom(getter_AddRefs(sideCSSValue));
        if (NS_FAILED(result))
          break;
        result = sideCSSValue->GetCssText(sideValue);
        if (NS_FAILED(result))
          break;
        tmpStr.Append(sideValue + comma);
        // get the left
        result = mValue.mRect->GetLeft(getter_AddRefs(sideCSSValue));
        if (NS_FAILED(result))
          break;
        result = sideCSSValue->GetCssText(sideValue);
        if (NS_FAILED(result))
          break;
        tmpStr.Append(sideValue + NS_LITERAL_STRING(")"));
        break;
      }
    case CSS_RGBCOLOR :
      {
        NS_ASSERTION(mValue.mColor, "mValue.mColor should never be null");
        NS_NAMED_LITERAL_STRING(comma, ", ");
        nsCOMPtr<nsIDOMCSSPrimitiveValue> colorCSSValue;
        nsAutoString colorValue;
        if (mValue.mColor->HasAlpha())
          tmpStr.AssignLiteral("rgba(");
        else
          tmpStr.AssignLiteral("rgb(");

        // get the red component
        result = mValue.mColor->GetRed(getter_AddRefs(colorCSSValue));
        if (NS_FAILED(result))
          break;
        result = colorCSSValue->GetCssText(colorValue);
        if (NS_FAILED(result))
          break;
        tmpStr.Append(colorValue + comma);

        // get the green component
        result = mValue.mColor->GetGreen(getter_AddRefs(colorCSSValue));
        if (NS_FAILED(result))
          break;
        result = colorCSSValue->GetCssText(colorValue);
        if (NS_FAILED(result))
          break;
        tmpStr.Append(colorValue + comma);

        // get the blue component
        result = mValue.mColor->GetBlue(getter_AddRefs(colorCSSValue));
        if (NS_FAILED(result))
          break;
        result = colorCSSValue->GetCssText(colorValue);
        if (NS_FAILED(result))
          break;
        tmpStr.Append(colorValue);

        if (mValue.mColor->HasAlpha()) {
          // get the alpha component
          result = mValue.mColor->GetAlpha(getter_AddRefs(colorCSSValue));
          if (NS_FAILED(result))
            break;
          result = colorCSSValue->GetCssText(colorValue);
          if (NS_FAILED(result))
            break;
          tmpStr.Append(comma + colorValue);
        }

        tmpStr.Append(NS_LITERAL_STRING(")"));

        break;
      }
    case CSS_S :
      {
        tmpStr.AppendFloat(mValue.mFloat);
        tmpStr.AppendLiteral("s");
        break;
      }
    case CSS_CM :
    case CSS_MM :
    case CSS_IN :
    case CSS_PT :
    case CSS_PC :
    case CSS_UNKNOWN :
    case CSS_EMS :
    case CSS_EXS :
    case CSS_DEG :
    case CSS_RAD :
    case CSS_GRAD :
    case CSS_MS :
    case CSS_HZ :
    case CSS_KHZ :
    case CSS_DIMENSION :
      NS_ERROR("We have a bogus value set.  This should not happen");
      return NS_ERROR_DOM_INVALID_ACCESS_ERR;
  }

  if (NS_SUCCEEDED(result)) {
    aCssText.Assign(tmpStr);
  }

  return NS_OK;
}


NS_IMETHODIMP
nsROCSSPrimitiveValue::SetCssText(const nsAString& aCssText)
{
  return NS_ERROR_DOM_NO_MODIFICATION_ALLOWED_ERR;
}


NS_IMETHODIMP
nsROCSSPrimitiveValue::GetCssValueType(uint16_t* aValueType)
{
  NS_ENSURE_ARG_POINTER(aValueType);
  *aValueType = nsIDOMCSSValue::CSS_PRIMITIVE_VALUE;
  return NS_OK;
}


// nsIDOMCSSPrimitiveValue

NS_IMETHODIMP
nsROCSSPrimitiveValue::GetPrimitiveType(uint16_t* aPrimitiveType)
{
  NS_ENSURE_ARG_POINTER(aPrimitiveType);
  *aPrimitiveType = mType;

  return NS_OK;
}


NS_IMETHODIMP
nsROCSSPrimitiveValue::SetFloatValue(uint16_t aUnitType, float aFloatValue)
{
  return NS_ERROR_DOM_NO_MODIFICATION_ALLOWED_ERR;
}


NS_IMETHODIMP
nsROCSSPrimitiveValue::GetFloatValue(uint16_t aUnitType, float* aReturn)
{
  NS_ENSURE_ARG_POINTER(aReturn);
  *aReturn = 0;

  switch(aUnitType) {
    case CSS_PX :
      if (mType != CSS_PX)
        return NS_ERROR_DOM_INVALID_ACCESS_ERR;
      *aReturn = nsPresContext::AppUnitsToFloatCSSPixels(mValue.mAppUnits);
      break;
    case CSS_CM :
      if (mType != CSS_PX)
        return NS_ERROR_DOM_INVALID_ACCESS_ERR;
      *aReturn = mValue.mAppUnits * CM_PER_INCH_FLOAT /
        nsPresContext::AppUnitsPerCSSInch();
      break;
    case CSS_MM :
      if (mType != CSS_PX)
        return NS_ERROR_DOM_INVALID_ACCESS_ERR;
      *aReturn = mValue.mAppUnits * MM_PER_INCH_FLOAT /
        nsPresContext::AppUnitsPerCSSInch();
      break;
    case CSS_IN :
      if (mType != CSS_PX)
        return NS_ERROR_DOM_INVALID_ACCESS_ERR;
      *aReturn = mValue.mAppUnits / nsPresContext::AppUnitsPerCSSInch();
      break;
    case CSS_PT :
      if (mType != CSS_PX)
        return NS_ERROR_DOM_INVALID_ACCESS_ERR;
      *aReturn = mValue.mAppUnits * POINTS_PER_INCH_FLOAT / 
        nsPresContext::AppUnitsPerCSSInch();
      break;
    case CSS_PC :
      if (mType != CSS_PX)
        return NS_ERROR_DOM_INVALID_ACCESS_ERR;
      *aReturn = mValue.mAppUnits * 6.0f /
        nsPresContext::AppUnitsPerCSSInch();
      break;
    case CSS_PERCENTAGE :
      if (mType != CSS_PERCENTAGE)
        return NS_ERROR_DOM_INVALID_ACCESS_ERR;
      *aReturn = mValue.mFloat * 100;
      break;
    case CSS_NUMBER :
      if (mType != CSS_NUMBER)
        return NS_ERROR_DOM_INVALID_ACCESS_ERR;
      *aReturn = mValue.mFloat;
      break;
    case CSS_UNKNOWN :
    case CSS_EMS :
    case CSS_EXS :
    case CSS_DEG :
    case CSS_RAD :
    case CSS_GRAD :
    case CSS_MS :
    case CSS_S :
    case CSS_HZ :
    case CSS_KHZ :
    case CSS_DIMENSION :
    case CSS_STRING :
    case CSS_URI :
    case CSS_IDENT :
    case CSS_ATTR :
    case CSS_COUNTER :
    case CSS_RECT :
    case CSS_RGBCOLOR :
      return NS_ERROR_DOM_INVALID_ACCESS_ERR;
  }

  return NS_OK;
}


NS_IMETHODIMP
nsROCSSPrimitiveValue::SetStringValue(uint16_t aStringType,
                                      const nsAString& aStringValue)
{
  return NS_ERROR_DOM_NO_MODIFICATION_ALLOWED_ERR;
}


NS_IMETHODIMP
nsROCSSPrimitiveValue::GetStringValue(nsAString& aReturn)
{
  switch (mType) {
    case CSS_IDENT:
      CopyUTF8toUTF16(nsCSSKeywords::GetStringValue(mValue.mKeyword), aReturn);
      break;
    case CSS_STRING:
    case CSS_ATTR:
      aReturn.Assign(mValue.mString);
      break;
    case CSS_URI: {
      nsAutoCString spec;
      if (mValue.mURI)
        mValue.mURI->GetSpec(spec);
      CopyUTF8toUTF16(spec, aReturn);
      } break;
    default:
      aReturn.Truncate();
      return NS_ERROR_DOM_INVALID_ACCESS_ERR;
  }
  return NS_OK;
}


NS_IMETHODIMP
nsROCSSPrimitiveValue::GetCounterValue(nsIDOMCounter** aReturn)
{
  return NS_ERROR_DOM_NO_MODIFICATION_ALLOWED_ERR;
}


NS_IMETHODIMP
nsROCSSPrimitiveValue::GetRectValue(nsIDOMRect** aReturn)
{
  if (mType != CSS_RECT) {
    *aReturn = nullptr;
    return NS_ERROR_DOM_INVALID_ACCESS_ERR;
  }
  NS_ASSERTION(mValue.mRect, "mValue.mRect should never be null");
  NS_ADDREF(*aReturn = mValue.mRect);
  return NS_OK;
}


NS_IMETHODIMP 
nsROCSSPrimitiveValue::GetRGBColorValue(nsIDOMRGBColor** aReturn)
{
  if (mType != CSS_RGBCOLOR) {
    *aReturn = nullptr;
    return NS_ERROR_DOM_INVALID_ACCESS_ERR;
  }
  NS_ASSERTION(mValue.mColor, "mValue.mColor should never be null");
  NS_ADDREF(*aReturn = mValue.mColor);
  return NS_OK;
}

void
nsROCSSPrimitiveValue::SetNumber(float aValue)
{
    Reset();
    mValue.mFloat = aValue;
    mType = CSS_NUMBER;
}

void
nsROCSSPrimitiveValue::SetNumber(int32_t aValue)
{
  Reset();
  mValue.mFloat = float(aValue);
  mType = CSS_NUMBER;
}

void
nsROCSSPrimitiveValue::SetNumber(uint32_t aValue)
{
  Reset();
  mValue.mFloat = float(aValue);
  mType = CSS_NUMBER;
}

void
nsROCSSPrimitiveValue::SetPercent(float aValue)
{
  Reset();
  mValue.mFloat = aValue;
  mType = CSS_PERCENTAGE;
}

void
nsROCSSPrimitiveValue::SetAppUnits(nscoord aValue)
{
  Reset();
  mValue.mAppUnits = aValue;
  mType = CSS_PX;
}

void
nsROCSSPrimitiveValue::SetAppUnits(float aValue)
{
  SetAppUnits(NSToCoordRound(aValue));
}

void
nsROCSSPrimitiveValue::SetIdent(nsCSSKeyword aKeyword)
{
  NS_PRECONDITION(aKeyword != eCSSKeyword_UNKNOWN &&
                  0 <= aKeyword && aKeyword < eCSSKeyword_COUNT,
                  "bad keyword");
  Reset();
  mValue.mKeyword = aKeyword;
  mType = CSS_IDENT;
}

// FIXME: CSS_STRING should imply a string with "" and a need for escaping.
void
nsROCSSPrimitiveValue::SetString(const nsACString& aString, uint16_t aType)
{
  Reset();
  mValue.mString = ToNewUnicode(aString);
  if (mValue.mString) {
    mType = aType;
  } else {
    // XXXcaa We should probably let the caller know we are out of memory
    mType = CSS_UNKNOWN;
  }
}

// FIXME: CSS_STRING should imply a string with "" and a need for escaping.
void
nsROCSSPrimitiveValue::SetString(const nsAString& aString, uint16_t aType)
{
  Reset();
  mValue.mString = ToNewUnicode(aString);
  if (mValue.mString) {
    mType = aType;
  } else {
    // XXXcaa We should probably let the caller know we are out of memory
    mType = CSS_UNKNOWN;
  }
}

void
nsROCSSPrimitiveValue::SetURI(nsIURI *aURI)
{
  Reset();
  mValue.mURI = aURI;
  NS_IF_ADDREF(mValue.mURI);
  mType = CSS_URI;
}

void
nsROCSSPrimitiveValue::SetColor(nsDOMCSSRGBColor* aColor)
{
  NS_PRECONDITION(aColor, "Null RGBColor being set!");
  Reset();
  mValue.mColor = aColor;
  if (mValue.mColor) {
    NS_ADDREF(mValue.mColor);
    mType = CSS_RGBCOLOR;
  }
  else {
    mType = CSS_UNKNOWN;
  }
}

void
nsROCSSPrimitiveValue::SetRect(nsIDOMRect* aRect)
{
  NS_PRECONDITION(aRect, "Null rect being set!");
  Reset();
  mValue.mRect = aRect;
  if (mValue.mRect) {
    NS_ADDREF(mValue.mRect);
    mType = CSS_RECT;
  }
  else {
    mType = CSS_UNKNOWN;
  }
}

void
nsROCSSPrimitiveValue::SetTime(float aValue)
{
  Reset();
  mValue.mFloat = aValue;
  mType = CSS_S;
}

void
nsROCSSPrimitiveValue::Reset()
{
  switch (mType) {
    case CSS_IDENT:
      break;
    case CSS_STRING:
    case CSS_ATTR:
    case CSS_COUNTER: // FIXME: Counter should use an object
      NS_ASSERTION(mValue.mString, "Null string should never happen");
      nsMemory::Free(mValue.mString);
      mValue.mString = nullptr;
      break;
    case CSS_URI:
      NS_IF_RELEASE(mValue.mURI);
      break;
    case CSS_RECT:
      NS_ASSERTION(mValue.mRect, "Null Rect should never happen");
      NS_RELEASE(mValue.mRect);
      break;
    case CSS_RGBCOLOR:
      NS_ASSERTION(mValue.mColor, "Null RGBColor should never happen");
      NS_RELEASE(mValue.mColor);
      break;
  }
}
