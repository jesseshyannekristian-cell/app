import asyncio
from playwright.async_api import async_playwright


async def main():
    async with async_playwright() as p:
        b = await p.chromium.launch(args=["--no-sandbox"])
        pg = await b.new_page(viewport={"width": 430, "height": 920})
        logs = []
        pg.on("console", lambda m: logs.append(f"{m.type}: {m.text}"))
        pg.on("pageerror", lambda e: logs.append(f"PAGEERROR: {e}"))
        await pg.goto("http://localhost:3000", wait_until="networkidle", timeout=120000)
        await pg.wait_for_timeout(8000)
        print("HOME:", (await pg.inner_text("body"))[:600])
        await pg.screenshot(path="/tmp/ic_home.png")
        try:
            await pg.click("[data-testid='nav-breach']", timeout=8000)
            await pg.wait_for_timeout(2000)
            await pg.click("[data-testid='breach-target-0']", timeout=8000)
            await pg.wait_for_timeout(1500)
            print("ASSESS:", (await pg.inner_text("body"))[:300])
            await pg.screenshot(path="/tmp/ic_breach.png")
            await pg.click("[data-testid='deploy-btn']", timeout=8000)
            await pg.wait_for_timeout(1500)
            await pg.screenshot(path="/tmp/ic_decision.png")
        except Exception as e:
            print("NAV ERR:", e)
        await pg.click("[data-testid='back-btn']", timeout=4000) if False else None
        print("LOGS:", " | ".join(logs[-12:]))
        await b.close()


asyncio.run(main())
