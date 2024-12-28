package app.organicmaps.util;

import android.content.Context;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatDelegate;
import app.organicmaps.Framework;
import app.organicmaps.R;
import app.organicmaps.display.DisplayManager;
import app.organicmaps.routing.RoutingController;

public enum ThemeSwitcher
{
  INSTANCE;

  private static boolean mRendererActive = false;

//  private final Runnable mAutoThemeChecker = new Runnable()
//  {
//    @Override
//    public void run()
//    {
//      String nightTheme = MwmApplication.from(mContext).getString(R.string.theme_night);
//      String defaultTheme = MwmApplication.from(mContext).getString(R.string.theme_default);
//      String theme = defaultTheme;
//      Location last = LocationHelper.from(mContext).getSavedLocation();
//
//      boolean navAuto = RoutingController.get().isNavigating() && ThemeUtils.isNavAutoTheme(mContext);
//
//      if (navAuto || ThemeUtils.isAutoTheme(mContext))
//      {
//        if (last == null)
//          theme = Config.getCurrentUiTheme(mContext);
//        else
//        {
//          long currentTime = System.currentTimeMillis() / 1000;
//          boolean day = Framework.nativeIsDayTime(currentTime, last.getLatitude(), last.getLongitude());
//          theme = (day ? defaultTheme : nightTheme);
//        }
//      }
//
//      setThemeAndMapStyle(theme);
//      UiThread.cancelDelayedTasks(mAutoThemeChecker);
//
//      if (navAuto || ThemeUtils.isAutoTheme(mContext))
//        UiThread.runLater(mAutoThemeChecker, CHECK_INTERVAL_MS);
//    }
//  };

  @SuppressWarnings("NotNullFieldNotInitialized")
  @NonNull
  private Context mContext;

  public void initialize(@NonNull Context context)
  {
    mContext = context;
  }

  /**
   * Changes the UI theme of application and the map style if necessary. If the contract regarding
   * the input parameter is broken, the UI will be frozen during attempting to change the map style
   * through the synchronous method {@link Framework#nativeSetMapStyle(int)}.
   *
   * @param isRendererActive Indicates whether OpenGL renderer is active or not. Must be
   *                         <code>true</code> only if the map is rendered and visible on the screen
   *                         at this moment, otherwise <code>false</code>.
   */
  @androidx.annotation.UiThread
  public void restart(boolean isRendererActive)
  {
    mRendererActive = isRendererActive;
    String storedTheme = Config.getThemeSettings(mContext); // follow-system etc

    int currentMapStyle = Framework.nativeGetMapStyle();
    // Resolve dynamic themes (follow-system, nav-auto etc.) to light or dark
    int resolvedTheme = getAndroidTheme(storedTheme);
    // Then derive map style from that, but handle debug commands
    // If current style is different to the style from theme, only set theme
    // to handle debug commands
    if (currentMapStyle != getMapStyle(storedTheme))
      setMapStyle(getMapStyle(storedTheme));

    setAndroidTheme(storedTheme);
  }

  private void setAndroidTheme(@NonNull String theme)
  {
    if (ThemeUtils.isSystemTheme(mContext, theme))
      AppCompatDelegate.setDefaultNightMode(AppCompatDelegate.MODE_NIGHT_FOLLOW_SYSTEM);
    else if (ThemeUtils.isAutoTheme(mContext, theme))
      {
        //TODO: Proper behaviour
        AppCompatDelegate.setDefaultNightMode(AppCompatDelegate.MODE_NIGHT_NO);
      }
    else if (ThemeUtils.isNavAutoTheme(mContext, theme))
      {
        //TODO: Proper behaviour
        AppCompatDelegate.setDefaultNightMode(AppCompatDelegate.MODE_NIGHT_NO);
      }
    else if (ThemeUtils.isNightTheme(mContext, theme))
      AppCompatDelegate.setDefaultNightMode(AppCompatDelegate.MODE_NIGHT_YES);
    else if (ThemeUtils.isDefaultTheme(mContext, theme))
      AppCompatDelegate.setDefaultNightMode(AppCompatDelegate.MODE_NIGHT_NO);
  }

  private void setMapStyle(@Framework.MapStyle int style)
  {
    // Because of the distinct behavior in auto theme, Android Auto employs its own mechanism for theme switching.
    // For the Android Auto theme switcher, please consult the app.organicmaps.car.util.ThemeUtils module.
    if (DisplayManager.from(mContext).isCarDisplayUsed())
      return;
    // If rendering is not active we can mark map style, because all graphics
    // will be recreated after rendering activation.
    if (mRendererActive)
      Framework.nativeSetMapStyle(style);
    else
      Framework.nativeMarkMapStyle(style);
  }

  private int getAndroidTheme(@NonNull String theme)
  {
    //TODO: Dynamic themes processing here, or passthrough for normal ones
    return R.string.theme_default;
  }

  private int getMapStyle(@NonNull String theme)
  {
    @Framework.MapStyle
    int style;
    if (ThemeUtils.isNightTheme(mContext, theme))
    {
      if (RoutingController.get().isVehicleNavigation())
        style = Framework.MAP_STYLE_VEHICLE_DARK;
      else if (Framework.nativeIsOutdoorsLayerEnabled())
        style = Framework.MAP_STYLE_OUTDOORS_DARK;
      else
        style = Framework.MAP_STYLE_DARK;
    }
    else
    {
      if (RoutingController.get().isVehicleNavigation())
        style = Framework.MAP_STYLE_VEHICLE_CLEAR;
      else if (Framework.nativeIsOutdoorsLayerEnabled())
        style = Framework.MAP_STYLE_OUTDOORS_CLEAR;
      else
        style = Framework.MAP_STYLE_CLEAR;
    }

    return style;
  }

//  private void setThemeAndMapStyle(@NonNull String theme, @Framework.MapStyle int style)
//  {
//    String oldTheme = Config.getCurrentUiTheme(mContext);
//
//    if (!theme.equals(oldTheme))
//    {
//      Config.setCurrentUiTheme(mContext, theme);
//      DownloaderStatusIcon.clearCache();
//
//      final Activity a = MwmApplication.from(mContext).getTopActivity();
//      if (a != null && !a.isFinishing())
//        a.recreate();
//    }
//    else
//    {
//      // If the UI theme is not changed we just need to change the map style if needed.
//      int currentStyle = Framework.nativeGetMapStyle();
//      if (currentStyle == style)
//        return;
//      SetMapStyle(style);
//    }
//  }
}
